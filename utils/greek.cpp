/**
 * @file    greek.cpp
 * @author  Thomas Mueller
 *
 * This file is part of GeodesicView.
 */
#include "greek.h"
#include <QString>

GreekLetter::GreekLetter()
{
    init();
}

GreekLetter::~GreekLetter()
{
}

QChar GreekLetter::toChar(QString greekName)
{
    QHash<QString, QChar>::const_iterator i = mCharHash.find(greekName);

    if (i != mCharHash.end()) {
        return i.value();
    } else if (greekName.size() == 1) {
        return greekName.at(0);
    } else {
        return QChar();
    }
}

void GreekLetter::init()
{
    mCharHash.insert(QString("alpha"), char_alpha);
    mCharHash.insert(QString("beta"), char_beta);
    mCharHash.insert(QString("gamma"), char_gamma);
    mCharHash.insert(QString("delta"), char_delta);
    mCharHash.insert(QString("epsilon"), char_epsilon);
    mCharHash.insert(QString("zeta"), char_zeta);
    mCharHash.insert(QString("eta"), char_eta);
    mCharHash.insert(QString("theta"), char_theta);
    mCharHash.insert(QString("jota"), char_jota);
    mCharHash.insert(QString("kappa"), char_kappa);
    mCharHash.insert(QString("lambda"), char_lambda);
    mCharHash.insert(QString("mu"), char_mu);
    mCharHash.insert(QString("nu"), char_nu);
    mCharHash.insert(QString("xi"), char_xi);
    mCharHash.insert(QString("omicron"), char_omicron);
    mCharHash.insert(QString("pi"), char_pi);
    mCharHash.insert(QString("rho"), char_rho);
    mCharHash.insert(QString("sigma"), char_sigma);
    mCharHash.insert(QString("tau"), char_tau);
    mCharHash.insert(QString("upsilon"), char_upsilon);
    mCharHash.insert(QString("phi"), char_phi);
    mCharHash.insert(QString("chi"), char_chi);
    mCharHash.insert(QString("psi"), char_psi);
    mCharHash.insert(QString("omega"), char_omega);
    mCharHash.insert(QString("Alpha"), char_Alpha);
    mCharHash.insert(QString("Beta"), char_Beta);
    mCharHash.insert(QString("Gamma"), char_Gamma);
    mCharHash.insert(QString("Delta"), char_Delta);
    mCharHash.insert(QString("Epsilon"), char_Epsilon);
    mCharHash.insert(QString("Zeta"), char_Zeta);
    mCharHash.insert(QString("Eta"), char_Eta);
    mCharHash.insert(QString("Theta"), char_Theta);
    mCharHash.insert(QString("Jota"), char_Jota);
    mCharHash.insert(QString("Kappa"), char_Kappa);
    mCharHash.insert(QString("Lambda"), char_Lambda);
    mCharHash.insert(QString("Mu"), char_Mu);
    mCharHash.insert(QString("Nu"), char_Nu);
    mCharHash.insert(QString("Xi"), char_Xi);
    mCharHash.insert(QString("Omicron"), char_Omicron);
    mCharHash.insert(QString("Pi"), char_Pi);
    mCharHash.insert(QString("Rho"), char_Rho);
    mCharHash.insert(QString("Sigma"), char_Sigma);
    mCharHash.insert(QString("Tau"), char_Tau);
    mCharHash.insert(QString("Upsilon"), char_Upsilon);
    mCharHash.insert(QString("Phi"), char_Phi);
    mCharHash.insert(QString("Chi"), char_Chi);
    mCharHash.insert(QString("Psi"), char_Psi);
    mCharHash.insert(QString("Omega"), char_Omega);
}
