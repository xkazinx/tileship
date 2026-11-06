#!/usr/bin/env bash
# Installs tileship on the Ubuntu box from an uploaded folder (deploy_ubuntu.ps1
# sends it to /tmp/tileship-upload): the server under /opt/tileship/bin as the
# service tileship (system user tileship), the web build under /opt/tileship/web,
# Caddy's snippet in /etc/caddy/sites/kazin.dev.d/ (Caddy restarted only when the
# snippet changed: its admin API is off).
# The database, once: MySQL's database tileship and its user tileship@localhost
# with a random password, written only to /opt/tileship/bin/init.json (mode 600,
# owned by tileship). A later install keeps both (the server makes its tables).
#   sudo bash install.sh [<upload folder>]
set -euo pipefail
SRC="${1:-/tmp/tileship-upload}"
SNIPPET=/etc/caddy/sites/kazin.dev.d/tileship.caddy
INIT=/opt/tileship/bin/init.json

id tileship >/dev/null 2>&1 || useradd --system --no-create-home --shell /usr/sbin/nologin tileship

# The MySQL client library the server links (libmysqlclient-dev's runtime)
if ! ldconfig -p | grep -q 'libmysqlclient.so.24'; then
	DEBIAN_FRONTEND=noninteractive apt-get install -y -qq libmysqlclient24 >/dev/null
fi

install -d -m 755 /opt/tileship /opt/tileship/bin
install -m 755 "$SRC/tileship_server" /opt/tileship/bin/tileship_server.new
mv -f /opt/tileship/bin/tileship_server.new /opt/tileship/bin/tileship_server

# The database and its account, made once (the password never printed)
if [ ! -f "$INIT" ]; then
	PW="$(openssl rand -hex 24)"
	mysql --protocol=socket -uroot <<-SQL
		CREATE DATABASE IF NOT EXISTS \`tileship\` CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci;
		CREATE USER IF NOT EXISTS 'tileship'@'localhost' IDENTIFIED BY '${PW}';
		ALTER USER 'tileship'@'localhost' IDENTIFIED BY '${PW}';
		GRANT ALL PRIVILEGES ON \`tileship\`.* TO 'tileship'@'localhost';
		FLUSH PRIVILEGES;
	SQL
	umask 077
	cat > "$INIT.new" <<-JSON
		{
		  "db_host": "localhost",
		  "db_user": "tileship",
		  "db_pass": "${PW}",
		  "db_name": "tileship"
		}
	JSON
	unset PW
	chown tileship:tileship "$INIT.new"
	chmod 600 "$INIT.new"
	mv -f "$INIT.new" "$INIT"
	echo "Database: tileship and tileship@localhost made, the account in $INIT"
fi

# The web build swapped whole (a visitor never gets half of two builds)
rm -rf /opt/tileship/web.new
cp -r "$SRC/web" /opt/tileship/web.new
chmod -R a+rX /opt/tileship/web.new
rm -rf /opt/tileship/web.old
[ -d /opt/tileship/web ] && mv /opt/tileship/web /opt/tileship/web.old
mv /opt/tileship/web.new /opt/tileship/web
rm -rf /opt/tileship/web.old

install -m 644 "$SRC/tileship.service" /etc/systemd/system/tileship.service
systemctl daemon-reload
systemctl enable tileship >/dev/null 2>&1
systemctl restart tileship

if ! cmp -s "$SRC/tileship.caddy" "$SNIPPET"; then
	[ -f "$SNIPPET" ] && cp "$SNIPPET" "$SNIPPET.bak"
	install -m 644 "$SRC/tileship.caddy" "$SNIPPET"
	if ! caddy validate --config /etc/caddy/Caddyfile --adapter caddyfile >/tmp/tileship-caddy.log 2>&1; then
		echo "(!) Caddy refused the snippet:"; tail -5 /tmp/tileship-caddy.log
		if [ -f "$SNIPPET.bak" ]; then mv -f "$SNIPPET.bak" "$SNIPPET"; else rm -f "$SNIPPET"; fi
		exit 1
	fi
	rm -f "$SNIPPET.bak"
	systemctl restart caddy
	echo "Caddy: snippet installed, restarted"
fi

sleep 1
systemctl is-active tileship
journalctl -u tileship -n 6 --no-pager -o cat
