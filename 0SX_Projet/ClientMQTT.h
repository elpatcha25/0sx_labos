#pragma once

#include <Arduino.h>
#include <WiFiEspAT.h>
#include <PubSubClient.h>
#include "Etats.h"

class ClientMQTT {
public:
    ClientMQTT(const char* ssid, const char* motDePasse, const char* numEtudiant);
    void demarrer();
    void mettreAJour(EtatPorte etatPorte, float distance,
                     EtatConvoyeur etatConv, int vitesse, ModeAffiche mode);
    ModeAffiche lireModePending();
    bool        urgencePending();
    void        resetPending();

private:
    WiFiClient   _wifiClient;
    PubSubClient _mqttClient;
    const char*  _ssid;
    const char*  _motDePasse;
    const char*  _numEtudiant;
    unsigned long _dernierEnvoi;
    unsigned long _derniereReconnexion;

    static const unsigned long INTERVALLE_ENVOI    = 2000;
    static const unsigned long INTERVALLE_RECONNEX = 5000;
    static const char*         MQTT_SERVEUR;
    static const int           MQTT_PORT;
    static const char*         MQTT_USER;
    static const char*         MQTT_PASS;

    static ModeAffiche  _modePending;
    static bool         _urgencePending;
    static ClientMQTT*  _instance;

    void _connecterWifi();
    void _connecterMQTT();
    void _envoyer(EtatPorte etatPorte, float distance,
                  EtatConvoyeur etatConv, int vitesse, ModeAffiche mode);
    static void _onMessage(char* topic, byte* payload, unsigned int length);
};
