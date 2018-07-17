#pragma once

#include <QtWidgets/QMainWindow>
#include <QAudioInput>
#include <QByteArray>
#include <QComboBox>
#include <QMainWindow>
#include <QObject>
#include <QPushButton>
#include <QWidget>
#include <QScopedPointer>
#include <qmessagebox.h>

#include "ui_spacjateleclient.h"
#include <iostream>
#include <string>
#include "rtpsession.h"
#include "rtppacket.h"
#include "rtpudpv4transmitter.h"
#include "rtpipv4address.h"
#include "rtpsessionparams.h"
#include "rtperrors.h"
#include "rtpsourcedata.h"
#include "rtplibraryversion.h"
#include <stdlib.h>
#include <stdio.h>
#include <jthread/jthread.h>


#include <math.h>
#include <QAudioOutput>
#include <QComboBox>
#include <QIODevice>
#include <QLabel>
#include <QSlider>
#include <QTimer>

class AudioInfo : public QIODevice
{
	Q_OBJECT

public:
	AudioInfo(const QAudioFormat &format);

	void start();
	void stop();

	qreal level() const { return m_level; }

	qint64 readData(char *data, qint64 maxlen) override;
	qint64 writeData(const char *data, qint64 len) override;

private:
	const QAudioFormat m_format;
	quint32 m_maxAmplitude = 0;
	qreal m_level = 0.0; // 0.0 <= m_level <= 1.0

signals:
	void update();
};

class MyRTPSession : public jrtplib::RTPSession
{
public:
	MyRTPSession();
	void OnPollThreadStep();
	void ProcessRTPPacket(const jrtplib::RTPSourceData &srcdat, const jrtplib::RTPPacket &rtppack);
	QByteArray arrayBuff;
};



class SpacjaTeleClient : public QMainWindow
{
	Q_OBJECT

public:
	SpacjaTeleClient(QWidget *parent = Q_NULLPTR);
	~SpacjaTeleClient();


public slots:
	void sendAudio();
	void muteAudioOut();
	void getAudio();


private:
	Ui::SpacjaTeleClientClass ui;
	QScopedPointer<AudioInfo> m_audioInfo;
	QScopedPointer<QAudioInput> m_audioInput;
	QScopedPointer<QAudioOutput> m_audioOutput;

	QTimer * m_pushTimer = nullptr;
	void audioInit(const QAudioDeviceInfo &deviceInfoIn, const QAudioDeviceInfo &deviceInfoOut);
	void rtpInit(std::string ipString = "127.0.0.1", uint16_t portIn = 1234, uint16_t portDes = 8080);
	void rtpServInit(uint16_t portIn = 8082);
	void disconnectAll();
	void changeChannel(int num);
	void onChangeChannel();
	void checkerror(int rtperr);


	WSADATA dat;
	jrtplib::RTPSession sess;
	jrtplib::RTPUDPv4TransmissionParams transparams;
	jrtplib::RTPSessionParams sessparams;
	uint16_t portbase;
	uint16_t destport;
	uint32_t destip;
	std::string ipstr;
	int status;

	WSADATA datServ;
	MyRTPSession sessServ;
	jrtplib::RTPUDPv4TransmissionParams transparamsServ;
	jrtplib::RTPSessionParams sessparamsServ;
	uint16_t portbaseServ;
	int statusServ;
};
