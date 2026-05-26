#include "ClientMQTT.h"

const char* ClientMQTT::MQTT_SERVEUR = "216.128.180.194";
const int   ClientMQTT::MQTT_PORT    = 1883;
const char* ClientMQTT::MQTT_USER    = "etdshawi";
const char* ClientMQTT::MQTT_PASS    = "shawi123";

ClientMQTT*  ClientMQTT::_instance       = nullptr;
ModeAffiche  ClientMQTT::_modePending    = MODE_NORMAL;
bool         ClientMQTT::_urgencePending = false;

ClientMQTT::ClientMQTT(const char* ssid, const char* motDePasse, const char* numEtudiant)
    : _mqttClient(_wifiClient),
      _ssid(ssid),
      _motDePasse(motDePasse),
      _numEtudiant(numEtudiant),
      _dernierEnvoi(0),
      _derniereReconnexion(0)
{
    _instance = this;
}

void ClientMQTT::demarrer() {
    _connecterWifi();
    _mqttClient.setServer(MQTT_SERVEUR, MQTT_PORT);
    _mqttClient.setCallback(_onMessage);
    _connecterMQTT();
}

void ClientMQTT::mettreAJour(EtatPorte etatPorte, float distance,
                              EtatConvoyeur etatConv, int vitesse, ModeAffiche mode) {
    unsigned long maintenant = millis();

    if (WiFi.status() != WL_CONNECTED) {
        if (maintenant - _derniereReconnexion >= INTERVALLE_RECONNEX) {
            _derniereReconnexion = maintenant;
            WiFi.begin(_ssid, _motDePasse);
        }
        return;
    }

    if (!_mqttClient.connected()) {
        if (maintenant - _derniereReconnexion >= INTERVALLE_RECONNEX) {
            _derniereReconnexion = maintenant;
            _connecterMQTT();
        }
        return;
    }

    _mqttClient.loop();

    if (maintenant - _dernierEnvoi >= INTERVALLE_ENVOI) {
        _dernierEnvoi = maintenant;
        _envoyer(etatPorte, distance, etatConv, vitesse, mode);
    }
}

ModeAffiche ClientMQTT::lireModePending()  { return _modePending; }
bool        ClientMQTT::urgencePending()   { return _urgencePending; }
void        ClientMQTT::resetPending()     { _urgencePending = false; _modePending = MODE_NORMAL; }

void ClientMQTT::_connecterWifi() {
    Serial3.begin(115200);
    WiFi.init(Serial3);

    if (WiFi.status() == WL_NO_MODULE) {
        Serial.println("Module WiFi introuvable!");
        return;
    }

    WiFi.disconnect();
    WiFi.setPersistent();
    WiFi.endAP();
    WiFi.begin(_ssid, _motDePasse);

    unsigned long debut = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - debut < 10000) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("WiFi connecte!");
    } else {
        Serial.println("WiFi echec.");
    }
}

void ClientMQTT::_connecterMQTT() {
    if (WiFi.status() != WL_CONNECTED) return;

    if (_mqttClient.connect(_numEtudiant, MQTT_USER, MQTT_PASS)) {
        Serial.println("MQTT connecte!");

        // S'abonner au topic de réception
        char topicSub[30];
        snprintf(topicSub, sizeof(topicSub), "etd/%s", _numEtudiant);
        _mqttClient.subscribe(topicSub);
        Serial.print("Abonne a: ");
        Serial.println(topicSub);
    } else {
        Serial.print("MQTT echec: ");
        Serial.println(_mqttClient.state());
    }
}

void ClientMQTT::_envoyer(EtatPorte etatPorte, float distance,
                           EtatConvoyeur etatConv, int vitesse, ModeAffiche mode) {
    const char* textePorte;
    switch (etatPorte) {
        case PORTE_OUVERTURE: textePorte = "ouverture"; break;
        case PORTE_OUVERTE:   textePorte = "ouverte";   break;
        case PORTE_FERMETURE: textePorte = "fermeture"; break;
        case PORTE_URGENCE:   textePorte = "urgence";   break;
        default:              textePorte = "fermee";    break;
    }

    const char* texteConv;
    switch (etatConv) {
        case CONV_MARCHE:  texteConv = "marche";  break;
        case CONV_URGENCE: texteConv = "urgence"; break;
        default:           texteConv = "arret";   break;
    }

    const char* texteMode;
    switch (mode) {
        case MODE_RABAIS:  texteMode = "rabais";  break;
        case MODE_ERREUR:  texteMode = "erreur";  break;
        case MODE_FERMER:  texteMode = "fermer";  break;
        case MODE_URGENCE: texteMode = "urgence"; break;
        default:           texteMode = "normal";  break;
    }

    char distStr[8];
    dtostrf(distance, 4, 1, distStr);

    char payload[200];
    snprintf(payload, sizeof(payload),
        "{\"porte\":\"%s\","
        "\"distance\":%s,"
        "\"convoyeur\":\"%s\","
        "\"vitesse\":%d,"
        "\"mode\":\"%s\"}",
        textePorte, distStr, texteConv, vitesse, texteMode
    );

    char topicPub[30];
    snprintf(topicPub, sizeof(topicPub), "etd/%s", _numEtudiant);

    _mqttClient.publish(topicPub, payload);
    Serial.print("Envoi: ");
    Serial.println(payload);
}

void ClientMQTT::_onMessage(char* topic, byte* payload, unsigned int length) {
    if (_instance == nullptr) return;

    char msg[64];
    unsigned int len = min(length, (unsigned int)(sizeof(msg) - 1));
    memcpy(msg, payload, len);
    msg[len] = '\0';

    Serial.print("Recu: ");
    Serial.println(msg);

    if      (strstr(msg, "urgence") != nullptr) _urgencePending = true;
    else if (strstr(msg, "rabais")  != nullptr) _modePending = MODE_RABAIS;
    else if (strstr(msg, "erreur")  != nullptr) _modePending = MODE_ERREUR;
    else if (strstr(msg, "fermer")  != nullptr) _modePending = MODE_FERMER;
    else if (strstr(msg, "normal")  != nullptr) _modePending = MODE_NORMAL;
}
