#include "EthernetOutputProvider.h"

#include "kdutils.h"

EthernetOutputProvider::EthernetOutputProvider(EthernetDevice* device, int amount)
	: m_device(device)
{
	for (int i = 0; i < amount; i++)
		m_outputs.push_back(0);
	m_lastUpdateTime = 0;
}

void EthernetOutputProvider::processData(const void* buffer, int len)
{
}
void EthernetOutputProvider::process()
{
	if (getTicks() - m_lastUpdateTime >= 2000)
		update();
}

void EthernetOutputProvider::setOutputState(int num, bool on)
{
	if (on == m_outputs[num])
		return;
	m_outputs[num] = on;
	update();
}
void EthernetOutputProvider::toggleOutputState(int num)
{
	m_outputs[num] = !m_outputs[num];
	update();
}

void EthernetOutputProvider::update()
{
	// TSrvOutputPacket p;
	// ByteBuffer data;
	// prepareCommand(data, 0x00);
	
	TSrvOutputSetOutputsPacket p;
	preparePacket((TSrvHeader*)&p, OUTPUT_CMD_SET_OUTPUTS);
	
	p.cnt = getAmount();
	p.outputs = 0;

	for (int i = 0; i < p.cnt; i++)
	{
		if (getOutputState(i))
			p.outputs |= (1 << i);
	}
	logInfo(str(Format("Sending output provider packet {} {}") << (uint64_t)this << p.outputs));
	// printf("out: 0x%02x\n", (unsigned int)b);
	
	sendData(&p, sizeof(p));
	m_lastUpdateTime = getTicks();
}
void EthernetOutputProvider::preparePacket(TSrvHeader* packet, uint8_t command)
{
	m_device->preparePacket(packet);
	packet->type = getType();
	packet->cmd = command;
}

string EthernetOutputProvider::getOutputID(int num)
{
	return str(Format("{}-{}") << getDevice()->getName() << num);
}

void EthernetOutputProvider::logInfo(const string& msg)
{
	logger->logInfo(Format("[{} - output] {}") << m_device->getName() << msg);
}
