#pragma once

namespace common
{
	//class Socket;

	namespace packet
	{
		class Out;
		class In;

		using OutPacketPtr = unique_ptr<Out>;
		using Size = enet_uint32;

		class Serializable
		{
		public:
			Serializable() = default;
			virtual ~Serializable() = default;

			virtual void Write(Out & out) const = 0;
			virtual void Read([[maybe_unused]] In * in) {} // To allow using Read in constructor

			virtual Size GetPacketSize() const = 0;
		};

#define SERIALIZE_IMPL(params)					\
			void Write(Out & out) const override	\
			{											\
				out.Write(params);						\
			}											\
			void Read(In * in) override			\
			{											\
				in->Read(params);						\
			}											\
			Size GetPacketSize() const override			\
			{											\
				return Sizer::Get(params);				\
			}

		class Base
		{
			char* _ptr;
			Size _size;

			virtual void Move(const Size sz) = 0;

		public:
			virtual ~Base() = default;
			Base(ENetPacket* packet, const Size sz);

			template<typename T = typename enable_if_t<!is_pointer_v<T>>>
			auto GetPointer() const { 
				return reinterpret_cast<T*>(_ptr); 
			}

			class Sizer
			{
			public:
				template<typename T, typename ... Args, typename = enable_if_t<!is_pointer_v<T>>>
				static Size Get(const T& val, const Args & ... args)
				{
					return _Get(val) + Get(forward<const Args>(args)...);
				}

			private:
				static Size Get() { return 0; }

				template <typename T, typename = enable_if_t<!is_pointer_v<T>>>
				static Size _Get(const T& val)
				{
					if constexpr (is_base_of_v<Serializable, T>)
					{
						return val.GetPacketSize();
					}
					else if constexpr (is_string<T>)
					{
						return sizeof(Size) + val.size();
					}
					/*else if constexpr (is_same_cr<T, char*>)
					{
						// #todo #net check if has to add +1 for null termination or something
						sz += sizeof(Size) + strlen(_data);
					} 
					else if constexpr(is_same_cr<T, EChar>)
					{
						sz += strlen(_data);
					}*/
					else
					{
						return sizeof(val);
					}
				}
			};

		protected:
			bool ProcessOverflow(const bool extra_condition);
			virtual bool HasOverflowed(const Size sz) = 0;

			Size GetSize() const { return _size; }
			Size& GetLength() const { return _enet_packet->dataLength; }
			void _Move(const Size pos, const bool is_out);

			ENetPacket* _enet_packet;
			bool _overflow;
		};

		class Out final : public Base
		{
		public:
			// Create by size and then write in another argument
			Out(const Size sz);
			~Out() override;

			void Move(const Size sz) override;
			bool HasOverflowed(const Size sz) override;

			// #todo packet::CreateStatic // packet::CreateDynamic
			template<typename T, typename = enable_if_t<!is_pointer_v<T>>, typename ... Args>
			static OutPacketPtr CreateStatic(const NetOp net_op, const T& val, const Args & ... args)
			{
				auto pkt = CreateDynamic(net_op, Sizer::Get(val, forward<const Args>(args)...));
				pkt->Write(val, forward<const Args>(args)...);
				return pkt;
			}

			static OutPacketPtr CreateDynamic(const NetOp net_op, const Size sz)
			{
				auto pkt = make_unique<Out>(sizeof(net_op) + sz);
				pkt->Write(net_op);
				return pkt;
			}

			void Send(ENetPeer* peer, const enet_uint8 channel = 0) const;

			template<typename T, typename = std::enable_if<std::is_pointer_v<T>>>
			void WriteFixed(T * src, unsigned long len) {
				memcpy(GetPointer<void>(), src, len);
				Move(len);
			}

			void WriteFixed(std::string src, unsigned long len)
			{
				if(src.size() < len)
					src.resize(len);

				WriteFixed((const char*)src.c_str(), len);
			}

			template<typename T, typename ... Args>
			void Write(const T & t, const Args & ...args)
			{
				_Write(t);
				Write(forward<const Args>(args)...);
			}

			template<typename T, std::size_t N>
			void Write(const array<T, N> & arr)
			{
				for (const auto & val : arr)
					Write(val);
			}

			template<typename T, std::size_t N>
			void Write(const T(&arr)[N])
			{
				for (std::size_t i = 0; i < N; i++)
					Write(arr[i]);
			}

		private:
			void Write() {}

			template<typename T, typename = enable_if_t<!is_pointer_v<T>>>
			void _Write(const T & v)
			{
				if constexpr (is_base_of_v<Serializable, T>)
				{
					v.Write(*this);
				}
				else if constexpr (is_string<T>)
				{
					auto sz = (Size)v.size();
					_Write(sz);

					if (HasOverflowed(sz))
					{
						return;
					}

					Print("Write string");
					std::copy(&v[0], &v[0] + sz, GetPointer<char>());
					Move(sz);
				}
				else
				{
					const auto sz = Sizer::Get(v);
					if (HasOverflowed(sz))
						return;

					auto ptr = GetPointer<T>();
					*ptr = v;
					Move(sz);
				}
			}
		};

		class In final : public Base
		{
		public:
			In(ENetPacket* p);
			~In() override;

			bool HasOverflowed(const Size sz) override;
			void Move(const Size sz) override;

			void ReadFixed(char * dest, unsigned long len) 
			{
				memcpy(dest, GetPointer<void>(), len);
				Move(len);
			}

			[[nodiscard]] std::string ReadFixed(unsigned long len)
			{
				std::string str(len, '\0');
				ReadFixed(str.data(), len);
				return str;
			}

			template<typename T, typename ... Args>
			void Read(T & t, Args & ...args)
			{
				_Read(t);
				Read(args...);
			}

			template<typename T, size_t N>
			void Read(std::array<T, N> & arr)
			{
				for (auto& val : arr)
					Read(val);
			}

			template<typename T, size_t N>
			void Read(T(&arr)[N])
			{
				for (std::size_t i = 0; i < N; i++)
					Read(arr[i]);
			}

			template<typename T>
			T ReadAs()
			{
				T value;
				Read(value);
				return value;
			}

		private:
			void Read() {}

			template<typename T, typename = enable_if_t<!is_pointer_v<T>>>
			void _Read(T & v)
			{
				if constexpr (is_base_of_v<Serializable, T>)
				{
					v.Read(this);
				}
				else if constexpr(is_string<T>)
				{
					Size sz;
					_Read(sz);

					v.resize(sz);
					std::copy(GetPointer<char>(), GetPointer<char>() + sz, &v[0]);
					Move(sz);
					Print("Read string");
				}
				else
				{
					const auto sz = Sizer::Get(v);
					if (HasOverflowed(sz))
						return;

					v = *GetPointer<T>();
					Move(sz);
				}
			}
		};
	}
	
	using InPkt = packet::In;
	using OutPkt = packet::Out;
	using OutPktRef = packet::OutPacketPtr&;
	using ESizer = packet::Base::Sizer;
}