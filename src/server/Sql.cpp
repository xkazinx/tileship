#include "stdafx.h"
#include "Sql.h"

Sql::Sql()
{
	mysql_init(Get());
}

Sql::~Sql()
{
	mysql_close(Get());
}

unique_ptr<Sql> Sql::Create()
{
	auto sql = make_unique<Sql>();

	const string host = lib::init::Value("db_host", "localhost");
	const string user = lib::init::Value("db_user", "tileship");
	const string pass = lib::init::Value("db_pass", "");
	const string name = lib::init::Value("db_name", "tileship");
	const string socket = lib::init::Value("db_socket", "");
	const auto port = (unsigned)lib::init::Number("db_port", 3306);

	// db_ssl: TLS to the database when it offers it. Off by default on Windows:
	// Connector/C 6.1 (setup.ps1) can't speak a MySQL 8 server's TLS (a local or
	// trusted database there); Linux's libmysqlclient can (and a local socket needs none)
#ifdef _WIN32
	constexpr bool kSslDefault = false;
#else
	constexpr bool kSslDefault = true;
#endif
	if (!lib::init::Flag("db_ssl", kSslDefault))
	{
		unsigned int mode = SSL_MODE_DISABLED;
		mysql_options(sql->Get(), MYSQL_OPT_SSL_MODE, &mode);
	}

	if (!mysql_real_connect(sql->Get(), host.c_str(), user.c_str(), pass.c_str(), name.c_str(), port,
		socket.empty() ? nullptr : socket.c_str(), 0))
	{
		Err("MySQL: can't connect to {}@{} ({}): {}", user, host, name, mysql_error(sql->Get()));
		return nullptr;
	}

	mysql_set_character_set(sql->Get(), "utf8mb4");
	return sql;
}

bool Sql::EnsureSchema()
{
	const auto sql = Create();
	if (!sql)
		return false;

	// The accounts: a name, a random salt and SHA2(salt + password, 256) in hex
	return sql->Query(
		"CREATE TABLE IF NOT EXISTS `players` ("
		"`uid` INT UNSIGNED NOT NULL AUTO_INCREMENT,"
		"`username` VARCHAR(32) NOT NULL,"
		"`salt` CHAR(32) NOT NULL,"
		"`password` CHAR(64) NOT NULL,"
		"`created` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,"
		"PRIMARY KEY (`uid`),"
		"UNIQUE KEY `username` (`username`)"
		") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");
}

unsigned Sql::FetchError()
{
	const auto err = mysql_errno(Get());
	if (err != 0)
		Err("MySQL {}: {}", err, mysql_error(Get()));
	return err;
}

bool Sql::Query(const string & query)
{
	if (mysql_real_query(Get(), query.data(), (unsigned long)query.size()) != 0)
	{
		FetchError();
		return false;
	}
	return true;
}

string Sql::Quote(const string & value)
{
	string out(value.size() * 2 + 1, '\0');
	const auto n = mysql_real_escape_string(Get(), out.data(), value.data(), (unsigned long)value.size());
	out.resize(n);
	return "'" + out + "'";
}

bool Sql::Insert(const string & table, const vector<string> & columns, const vector<vector<string>> & rows)
{
	assert(columns.size() > 0);

	if (rows.empty())
	{
		Err("Tried to run Sql::Insert without rows to insert");
		return false;
	}

	Info("Sql::Insert called for {} table", table);

	auto values = "("s;
	{
		for (szt i = 0; i < columns.size() - 1; i++)
		{
			values += Scape(columns[i], "`", ",");
		}

		values += Scape(columns[columns.size() - 1], "`", ")");
	}

	string data;
	{
		for (szt i = 0; i <= rows.size() - 1; i++)
		{
			const auto & row = rows[i];

			data += "(";

			for (szt j = 0; j < row.size(); j++)
			{
				data += Quote(row[j]);
				data += j == row.size() - 1 ? ")" : ",";
			}

			data += i == rows.size() - 1 ? ";" : ",";
		}
	}

	return Query(format("INSERT INTO `{}` {} VALUES {}", table, values, data));
}

unique_ptr<SelectResult> Sql::Select(const string & table, const vector<string> & columns, const string & extra)
{
	string select;

	for (szt i = 0; i < columns.size() - 1; i++)
	{
		select += Scape(columns[i], "`", ",");
	}

	select += Scape(columns[columns.size() - 1], "`");

	if (!Query(format("SELECT {} FROM `{}`{}", select, table, extra)))
		return nullptr;

	return make_unique<SelectResult>(this);
}

SelectResult::SelectResult(Sql * sql)
{
	_ptr = mysql_store_result(sql->Get());
	if (!_ptr)
		return;

	const auto field_count = mysql_num_fields(_ptr);
	{
		auto total = (SelectValue)mysql_num_rows(_ptr);
		_rows.reserve(total);
		_lengths.resize(total);
		while (total-- > 0)
		{
			_rows.emplace_back(mysql_fetch_row(_ptr));

			auto lengths = mysql_fetch_lengths(_ptr);
			for (szt i = 0; i < field_count; ++i)
			{
				_lengths[_rows.size() - 1].emplace_back(lengths[i]);
			}
		}
	}

	{
		mysql_field_seek(_ptr, 0);
		for (unsigned i = 0; i < field_count; ++i)
		{
			_fields.emplace_back(mysql_fetch_field(_ptr));
			_fields_indexes[_fields.back()->name] = i;
		}
	}
}

SelectResult::~SelectResult()
{
	if (_ptr)
		mysql_free_result(_ptr);
}
