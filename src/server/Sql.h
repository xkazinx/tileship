#pragma once

struct Sql;
using SelectValue = szt;

struct SelectItem
{
	SelectItem(const MYSQL_ROW & row, const vector<unsigned long> & length, const unordered_map<string, SelectValue> & fields)
		: _row(row), _length(length), _fields(fields)
	{
	}

	template <typename T>
	T Get(const string & field)
	{
		if (const auto it = _fields.find(field); it != _fields.end())
		{
			const auto idx = it->second;
			const string data = _row[idx] ? string(_row[idx], _length[idx]) : string();

			if constexpr (is_same_v<bool, T>)
			{
				return std::stoi(data) > 0;
			}
			else if constexpr (std::is_arithmetic_v<T>)
			{
				if constexpr (std::is_floating_point_v<T>)
					return (T)std::stod(data);
				else if constexpr (std::is_signed_v<T>)
					return (T)std::stoll(data);
				else
					return (T)std::stoull(data);
			}
			else
				return data;
		}

		Err("Field not found for SelectItem::Get: {}", field);
		return T{};
	}

	const MYSQL_ROW & _row;
	const vector<unsigned long> & _length;
	const unordered_map<string, SelectValue> & _fields;
};

struct SelectResult final
{
	explicit SelectResult(Sql * sql);
	~SelectResult();

	MYSQL_RES * _ptr;
	vector<MYSQL_ROW> _rows;
	vector<MYSQL_FIELD *> _fields;
	vector<vector<unsigned long>> _lengths;
	unordered_map<string, SelectValue> _fields_indexes;
	SelectValue _current_row_idx = 0;

	unique_ptr<SelectItem> Pop()
	{
		const auto idx = _current_row_idx++;
		if (idx >= _rows.size())
			return nullptr;

		return make_unique<SelectItem>(_rows[(szt)idx], _lengths[(szt)idx], _fields_indexes);
	}

	auto GetCount() const { return _rows.size(); }
	auto IsEmpty() const { return _rows.empty(); }
};

// One connection to the game's database. Where it is: the startup flags (init.json
// beside the exe, kept out of git: src/server/init.json.example) db_host
// ("localhost": the local socket on Linux), db_port (3306), db_user, db_pass,
// db_name ("tileship"), db_socket (a socket path; "" = the client library's own)
struct Sql final
{
	Sql();
	~Sql();
	Sql(const Sql &) = delete;
	Sql & operator=(const Sql &) = delete;

	// A connection, or nullptr (an Err line says why)
	static unique_ptr<Sql> Create();
	// The tables, made when missing (at the server's start)
	static bool EnsureSchema();

	MYSQL _ptr;

	auto Get() { return &_ptr; }

	unsigned FetchError();

	// false (and an Err line) when the server refused it
	bool Query(const string & query);
	bool Insert(const string & table, const vector<string> & columns, const vector<vector<string>> & rows);
	unique_ptr<SelectResult> Select(const string & table, const vector<string> & columns, const string & extra = "");

	// A value as an SQL string literal: escaped for this connection, in quotes
	string Quote(const string & value);

	static string Scape(const string & str, const string & scape, const string & add = "")
	{
		return scape + str + scape + add;
	}
};
