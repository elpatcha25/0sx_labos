#pragma once

enum EtatPorte : uint8_t {
    PORTE_FERMEE,
    PORTE_OUVERTURE,
    PORTE_OUVERTE,
    PORTE_FERMETURE,
    PORTE_URGENCE
};

enum EtatConvoyeur : uint8_t {
    CONV_ARRET,
    CONV_MARCHE,
    CONV_URGENCE
};

enum EtatCapteur : uint8_t {
    CAPTEUR_IDLE,
    CAPTEUR_TRIG,
    CAPTEUR_ATTENTE_ECHO,
    CAPTEUR_LECTURE
};

enum ModeAffiche : uint8_t {
    MODE_NORMAL,
    MODE_RABAIS,
    MODE_ERREUR,
    MODE_FERMER,
    MODE_URGENCE
};
