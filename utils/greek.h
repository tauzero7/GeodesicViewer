/**
 * @file    greek.h
 * @author  Thomas Mueller
 *
 * @brief  Greek letters
 *
 * This file is part of GeodesicView.
 */
#ifndef GREEK_LETTERS
#define GREEK_LETTERS

#include <QChar>
#include <QHash>
#include <string>

static QChar char_alpha(0x03B1);
static QChar char_beta(0x03B2);
static QChar char_gamma(0x03B3);
static QChar char_delta(0x03B4);
static QChar char_epsilon(0x03B5);
static QChar char_zeta(0x03B6);
static QChar char_eta(0x03B7);
static QChar char_theta(0x03B8);
static QChar char_jota(0x03B9);
static QChar char_kappa(0x03BA);
static QChar char_lambda(0x03BB);
static QChar char_mu(0x03BC);
static QChar char_nu(0x03BD);
static QChar char_xi(0x03BE);
static QChar char_omicron(0x03BF);
static QChar char_pi(0x03C0);
static QChar char_rho(0x03C1);
static QChar char_sigma(0x03C3);
static QChar char_tau(0x03C4);
static QChar char_upsilon(0x03C5);
static QChar char_phi(0x03C6);
static QChar char_chi(0x03C7);
static QChar char_psi(0x03C8);
static QChar char_omega(0x03C9);

//greek capital letters
static QChar char_Alpha(0x0391);
static QChar char_Beta(0x0392);
static QChar char_Gamma(0x0393);
static QChar char_Delta(0x0394);
static QChar char_Epsilon(0x0395);
static QChar char_Zeta(0x0396);
static QChar char_Eta(0x0397);
static QChar char_Theta(0x0398);
static QChar char_Jota(0x0399);
static QChar char_Kappa(0x039A);
static QChar char_Lambda(0x039B);
static QChar char_Mu(0x039C);
static QChar char_Nu(0x039D);
static QChar char_Xi(0x039E);
static QChar char_Omicron(0x039F);
static QChar char_Pi(0x03A0);
static QChar char_Rho(0x03A1);
static QChar char_Sigma(0x03A3);
static QChar char_Tau(0x03A4);
static QChar char_Upsilon(0x03A5);
static QChar char_Phi(0x03A6);
static QChar char_Chi(0x03A7);
static QChar char_Psi(0x03A8);
static QChar char_Omega(0x03A9);

class GreekLetter {
public:
    GreekLetter();
    ~GreekLetter();

    QChar toChar(QString greekName);

protected:
    void init();

private:
    QHash<QString, QChar> mCharHash;
};

#endif
