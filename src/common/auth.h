#pragma once

namespace common
{
	// A login / new account: two fixed fields of the name's / password's bytes and
	// a zero (player::kNameSize + 1, kPasswordSize + 1), as the game's first packet
	// layer laid them out
	struct Auth : lib::NetItem
	{
		static constexpr PktSz kUserBytes = player::kNameSize + 1;
		static constexpr PktSz kPassBytes = player::kPasswordSize + 1;
		static constexpr PktSz kPktMax = kUserBytes + kPassBytes;

		Auth() = default;
		Auth(const string & user, const string & pass) : _user(user), _pass(pass) {}
		explicit Auth(IPkt * const in) { Read(in); }

		void Write(OPkt & out) const override
		{
			out.WriteFixed(_user.substr(0, player::kNameSize), kUserBytes);
			out.WriteFixed(_pass.substr(0, player::kPasswordSize), kPassBytes);
		}

		void Read(IPkt * const in) override
		{
			_user = in->ReadFixed(kUserBytes);
			_pass = in->ReadFixed(kPassBytes);
		}

		PktSz GetPacketSize() const override { return kPktMax; }

		// A name of 1..kNameSize letters, digits or '_', a password of 1..kPasswordSize bytes
		bool IsValid() const
		{
			if (_user.empty() || _user.size() > player::kNameSize || _pass.empty() || _pass.size() > player::kPasswordSize)
				return false;
			for (const char c : _user)
				if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_'))
					return false;
			return true;
		}

		string _user, _pass;
	};
}
