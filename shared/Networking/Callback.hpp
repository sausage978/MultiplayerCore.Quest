#pragma once
#include "main.hpp"

#include "GlobalNamespace/ThreadStaticPacketPool_1.hpp"
#include "GlobalNamespace/PacketPool_1.hpp"
#include "LiteNetLib/Utils/NetDataReader.hpp"

template <class T>
using PacketCallback = void (*)(T, GlobalNamespace::IConnectedPlayer*);

namespace MultiplayerCore {
	class CallbackBase {
	public:
		virtual ~CallbackBase() {};
		virtual void Invoke(LiteNetLib::Utils::NetDataReader* reader, int size, GlobalNamespace::IConnectedPlayer* player) = 0;
	};

	template <class TPacket> class CallbackWrapper : public CallbackBase {
	private:
		PacketCallback<TPacket> action = nullptr;

	public:
		CallbackWrapper(PacketCallback<TPacket> callback) {
			action = callback;
		}

		void Invoke(LiteNetLib::Utils::NetDataReader* reader, int size, GlobalNamespace::IConnectedPlayer* player) {
			getLogger().debug("Running Invoke creating packet");
			//TPacket packet = THROW_UNLESS(il2cpp_utils::New<TPacket>());
			//getLogger().debug("Assigning from ThreadStaticPacketPool");
			//packet = GlobalNamespace::ThreadStaticPacketPool_1<TPacket>::get_pool()->Obtain();
			TPacket packet = GlobalNamespace::ThreadStaticPacketPool_1<TPacket>::get_pool()->Obtain();
			if (packet == nullptr) {
				reader->SkipBytes(size);
			}
			else {
				try {
					packet->Deserialize(reader);
				}
				catch (const std::exception& e) {
					getLogger().debug("Exception Deserializing Packet");
					getLogger().error("REPORT TO ENDER: %s", e.what());
				}
				catch (...) {
					getLogger().debug("REPORT TO ENDER: Unknown exception Deserializing Packet");
				}
			}
			if (action != nullptr) {
				try {
					action(packet, player);
				}
				catch (const std::exception& e) {
					getLogger().error("Exception running action");
					getLogger().error("REPORT TO ENDER: %s", e.what());
				}
				catch (...) {
					getLogger().debug("REPORT TO ENDER: Unknown exception Deserializing Packet");
				}
			}
		}
	};
}