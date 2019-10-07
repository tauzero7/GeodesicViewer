/**
 * @file    locted_view.cpp
 * @author  Thomas Mueller
 *
 * This file is part of GeodesicView.
 */
#include "locted_view.h"
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>

extern m4d::Object mObject;

LoctedView::LoctedView(QWidget* parent)
    : QGroupBox(parent)
{
    mGramSchmidt = new GramSchmidt();
    init();
    setLocalTetrad(true);
}

LoctedView::~LoctedView()
{
    delete mGramSchmidt;
}

void LoctedView::updateData()
{
    if (static_cast<int>(mObject.tetradType) >= 0
        && static_cast<unsigned int>(mObject.tetradType) < m4d::NUM_ENUM_NAT_TETRAD_TYPES) {
        cob_locted_type->setCurrentIndex(static_cast<int>(mObject.tetradType));
    }

    for (int i = 0; i < 4; i++) {
        lst_led_locted_pos_value[i]->setText(QString::number(mObject.startPos[i], 'f', DEF_PREC_POSITION));
    }

    for (int row = 0; row < 4; row++)
        for (int col = 0; col < 4; col++) {
            tbw_locted_tetrad->item(row, col)->setText(QString::number(mObject.base[row][col]));
        }
}

void LoctedView::resetAll()
{
    std::string coords[4] = { "t", "x", "y", "z" };
    for (int i = 0; i < 4; i++) {
        lst_lab_locted_coordname[i]->setText(coords[i].c_str());
        lst_led_locted_pos_value[i]->setText(QString::number(0.0, 'f', DEF_PREC_POSITION));
        lst_led_locted_pos_step[i]->setText(QString::number(DEF_VALUE_STEP));
    }
    resetBoost();
    slot_resetLocalTetrad();
}

void LoctedView::adjustMetric()
{
    if (mObject.currMetric == nullptr) {
        return;
    }

    m4d::vec4 pos = mObject.currMetric->getStandardInitialPosition();
    for (int i = 0; i < 4; i++) {
        lst_led_locted_pos_value[i]->setText(QString::number(pos[i], 'f', DEF_PREC_POSITION));
        lst_led_locted_pos_step[i]->setText(QString::number(DEF_VALUE_STEP));
    }
}

void LoctedView::adjustCoordNames()
{
    if (mObject.currMetric == nullptr) {
        return;
    }

    std::string coordNames[4];
    for (int i = 0; i < 4; i++) {
        coordNames[i] = std::string(mObject.currMetric->getCoordName(i));
    }

    for (int i = 0; i < 4; i++) {
        QChar ch = mGreekLetter.toChar(coordNames[i].c_str());
        if (ch == QChar()) {
            lst_lab_locted_coordname[i]->setText(coordNames[i].c_str());
        }
        else {
            lst_lab_locted_coordname[i]->setText(ch);
        }
    }
}

void LoctedView::adjustLocTedTypes()
{
    if (mObject.currMetric == nullptr) {
        return;
    }

    std::vector<m4d::enum_nat_tetrad_type> loctedtypes;
    int numLTT = mObject.currMetric->getLocTedTypes(loctedtypes);
    cob_locted_type->clear();
    for (int i = 0; i < numLTT; i++) {
        cob_locted_type->addItem(QString(m4d::stl_nat_tetrad_types[mObject.currMetric->getCurrLTtype(i)]));
    }

    if (cob_locted_predef->count() > 0) {
        cob_locted_predef->setCurrentIndex(0);
    }
}

void LoctedView::setLocalTetrad(bool reset)
{
    if (reset) {
        for (int i = 0; i < 4; i++) {
            mObject.base[i] = m4d::vec4();
            mObject.base[i].setX(i, 1.0);
        }
    }
    else {
        for (int row = 0; row < 4; row++)
            for (int col = 0; col < 4; col++) {
                bool ok;
                double val = tbw_locted_tetrad->item(row, col)->text().toDouble(&ok);
                if (ok) {
                    mObject.base[row].setX(col, val);
                }
            }
        for (int i = 0; i < 4; i++) {
            mObject.base[i].normalize();
        }
    }

    if (tbw_locted_tetrad != nullptr)
        for (int row = 0; row < 4; row++)
            for (int col = 0; col < 4; col++) {
                tbw_locted_tetrad->item(row, col)->setText(QString::number(mObject.base[row].x(col)));
            }
}

void LoctedView::setBoostParams(double chi, double ksi, double vel)
{
    led_boost_chi_value->setValue(chi);
    led_boost_ksi_value->setValue(ksi);
    double v = vel;
    if (fabs(v) > 1.0) {
        v = 1.0;
    }
    led_boost_vel_value->setValue(v);
    mObject.setLorentzTransf(chi, ksi, v);
}

void LoctedView::resetBoost()
{
    led_boost_chi_value->setValue(DEF_INIT_CHI);
    led_boost_ksi_value->setValue(DEF_INIT_KSI);
    led_boost_vel_value->setValue(0.0);

    led_boost_ksi_step->setValue(DEF_INIT_KSI_STEP);
    led_boost_chi_step->setValue(DEF_INIT_CHI_STEP);
    led_boost_vel_step->setValue(DEF_INIT_VEL_STEP);
    mObject.resetLorentzTransf();
}

// void LoctedView::addObjectsToScriptEngine(QScriptEngine* engine) {
//    QScriptValue lt = engine->newQObject(this);
//    engine->globalObject().setProperty("lt", lt);
//}

void LoctedView::slot_setPosition()
{
    double pos[4];
    for (int i = 0; i < 4; i++) {
        pos[i] = lst_led_locted_pos_value[i]->getValue();
    }

    mObject.startPos = m4d::vec4(pos[0], pos[1], pos[2], pos[3]);
    emit calcGeodesic();
}

void LoctedView::slot_setPosStep()
{
    double step;
    for (int i = 0; i < 4; i++) {
        step = lst_led_locted_pos_step[i]->getValue();
        lst_led_locted_pos_value[i]->setStep(step);
    }
}

void LoctedView::slot_decrPosition()
{
    QObject* obj = sender();
    bool ok;
    int num = obj->objectName().right(1).toInt(&ok);

    if (ok)
        if (num >= 0 && num < 4) {
            mObject.startPos[num] = lst_led_locted_pos_value[num]->text().toDouble();
            double step = lst_led_locted_pos_step[num]->text().toDouble();
            mObject.startPos[num] -= step;
            lst_led_locted_pos_value[num]->setText(QString::number(mObject.startPos[num], 'f', DEF_PREC_POSITION));
            emit calcGeodesic();
        }
}

void LoctedView::slot_incrPosition()
{
    QObject* obj = sender();
    bool ok;
    int num = obj->objectName().right(1).toInt(&ok);

    if (ok)
        if (num >= 0 && num < 4) {
            mObject.startPos[num] = lst_led_locted_pos_value[num]->text().toDouble();
            double step = lst_led_locted_pos_step[num]->text().toDouble();
            mObject.startPos[num] += step;
            lst_led_locted_pos_value[num]->setText(QString::number(mObject.startPos[num], 'f', DEF_PREC_POSITION));
            emit calcGeodesic();
        }
}

void LoctedView::slot_resetPosition()
{
    if (mObject.currMetric != nullptr) {
        mObject.startPos = mObject.currMetric->getStandardInitialPosition();
        for (int i = 0; i < 4; i++) {
            lst_led_locted_pos_value[i]->setText(QString::number(mObject.startPos[i], 'f', DEF_PREC_POSITION));
        }
        emit calcGeodesic();
    }
}

void LoctedView::slot_setLoctedType()
{
    if (mObject.currMetric == nullptr) {
        return;
    }

    m4d::enum_nat_tetrad_type type = mObject.currMetric->getCurrLTtype(cob_locted_type->currentIndex());
    mObject.tetradType = type;
    emit calcGeodesic();
}

void LoctedView::slot_resetLocalTetrad()
{
    cob_locted_predef->setCurrentIndex(0);
}

void LoctedView::slot_setLocalTetrad()
{
    setLocalTetrad(false);
    emit calcGeodesic();
}

void LoctedView::slot_orthoLocalTetrad()
{
    setLocalTetrad(false);
    mGramSchmidt->calculateTetrad(mObject.base[0], mObject.base[1], mObject.base[2], mObject.base[3]);
    // mGramSchmidt->print();

    for (int row = 0; row < 4; row++)
        for (int col = 0; col < 4; col++) {
            tbw_locted_tetrad->item(row, col)->setText(QString::number(mObject.base[row].x(col)));
        }
    emit calcGeodesic();
}

void LoctedView::slot_setPredefTetrad(int num)
{
    for (int row = 0; row < 4; row++) {
        for (int comp = 0; comp < 4; comp++) {
            mObject.base[row].setX(comp, m4d::vec_predef_tetrad[num][4 * row + comp]);
        }
        for (int col = 0; col < 4; col++) {
            tbw_locted_tetrad->item(row, col)->setText(QString::number(mObject.base[row].x(col)));
        }
    }
    emit calcGeodesic();
}

void LoctedView::slot_setBoostParams()
{
    double chi = led_boost_chi_value->getValue();
    double ksi = led_boost_ksi_value->getValue();
    double beta = led_boost_vel_value->getValue();
    mObject.setLorentzTransf(chi, ksi, beta);
    emit calcGeodesic();
}

void LoctedView::slot_setBoostParamSteps()
{
    led_boost_chi_value->setStep(led_boost_chi_step->getValue());
    led_boost_ksi_value->setStep(led_boost_ksi_step->getValue());
    led_boost_vel_value->setStep(led_boost_vel_step->getValue());
}

void LoctedView::slot_resetBoost()
{
    resetBoost();
    emit calcGeodesic();
}

void LoctedView::setPosition(int num, double val)
{
    if (num >= 0 && num < 4) {
        lst_led_locted_pos_value[num]->setValue(val);
    }

    double pos[4];
    for (int i = 0; i < 4; i++) {
        pos[i] = lst_led_locted_pos_value[i]->getValue();
    }

    mObject.startPos = m4d::vec4(pos[0], pos[1], pos[2], pos[3]);
    emit calcGeodesic();
}

void LoctedView::setPosition(double x0, double x1, double x2, double x3)
{
    double pos[4] = { x0, x1, x2, x3 };
    for (int i = 0; i < 4; i++) {
        lst_led_locted_pos_value[i]->setValue(pos[i]);
    }

    mObject.startPos = m4d::vec4(pos[0], pos[1], pos[2], pos[3]);
    emit calcGeodesic();
}

void LoctedView::setPredefTetrad(int num)
{
    if (num >= 0 && num < m4d::NUM_PREDEF_TETRADS) {
        cob_locted_predef->setCurrentIndex(num);
        slot_setPredefTetrad(num);
    }
}

void LoctedView::setPredefTetrad(QString name)
{
    int num = 0;
    while (num < m4d::NUM_PREDEF_TETRADS) {
        if (name.compare(QString(m4d::stl_predef_tetrads[num])) == 0) {
            cob_locted_predef->setCurrentIndex(num);
            slot_setPredefTetrad(num);
            break;
        }
        num++;
    }
}

void LoctedView::init()
{
    initElements();
    initGUI();
    initControl();
    initStatusTips();
}

void LoctedView::initElements()
{
    lab_value = new QLabel("value");
    lab_step = new QLabel("step");

    lab_locted_type = new QLabel(tr("Natural local tetrad"));
    cob_locted_type = new QComboBox();
    cob_locted_type->addItem(QString(m4d::stl_nat_tetrad_types[m4d::enum_nat_tetrad_default]));

    // ----------------------
    //    position
    // ----------------------
    wgt_locted_position = new QWidget();

    if (!lst_lab_locted_coordname.empty()) {
        lst_lab_locted_coordname.clear();
    }

    if (!lst_led_locted_pos_value.empty()) {
        lst_led_locted_pos_value.clear();
    }

    if (!lst_led_locted_pos_step.empty()) {
        lst_led_locted_pos_step.clear();
    }

    QStringList coordnames = QStringList() << "t"
                                           << "x"
                                           << "y"
                                           << "z";
    for (int i = 0; i < 4; i++) {
        QLabel* lab = new QLabel(coordnames[i]);
        lst_lab_locted_coordname.push_back(lab);

        DoubleEdit* led_pos = new DoubleEdit(DEF_PREC_POSITION);
        led_pos->setValidator(new QDoubleValidator(led_pos));
        DoubleEdit* led_step = new DoubleEdit(DEF_PREC_POSITION, DEF_VALUE_STEP);
        led_step->setValidator(new QDoubleValidator(led_step));

        lst_led_locted_pos_value.push_back(led_pos);
        lst_led_locted_pos_step.push_back(led_step);

        QPushButton* pub_pos_left = new QPushButton(QIcon(":down.png"), "");
        pub_pos_left->setObjectName(QString("pub_pos_left %1").arg(i));
        pub_pos_left->setAutoRepeat(true);
        pub_pos_left->setAutoRepeatDelay(400);
        lst_pub_locted_pos_decr.push_back(pub_pos_left);

        QPushButton* pub_pos_right = new QPushButton(QIcon(":up.png"), "");
        pub_pos_right->setObjectName(QString("pub_pos_right %1").arg(i));
        pub_pos_right->setAutoRepeat(true);
        pub_pos_right->setAutoRepeatDelay(400);
        lst_pub_locted_pos_incr.push_back(pub_pos_right);
    }
    pub_pos_reset = new QPushButton("reset");

    QGridLayout* layout_locted_position = new QGridLayout();
    layout_locted_position->addWidget(lab_value, 0, 1, Qt::AlignHCenter);
    layout_locted_position->addWidget(lab_step, 0, 4, Qt::AlignHCenter);
    for (int i = 0; i < 4; i++) {
        layout_locted_position->addWidget(lst_lab_locted_coordname[i], i + 1, 0);
        layout_locted_position->addWidget(lst_led_locted_pos_value[i], i + 1, 1);
        layout_locted_position->addWidget(lst_pub_locted_pos_incr[i], i + 1, 2);
        layout_locted_position->addWidget(lst_pub_locted_pos_decr[i], i + 1, 3);
        layout_locted_position->addWidget(lst_led_locted_pos_step[i], i + 1, 4);
    }
    layout_locted_position->addWidget(pub_pos_reset, 5, 1);
    layout_locted_position->setRowStretch(6, 10);
    wgt_locted_position->setLayout(layout_locted_position);

    // ----------------------
    //   tetrad direction
    // ----------------------
    tbw_locted_tetrad = new QTableWidget(4, 4);
    QStringList collistLT = QStringList()
        << QString("%1%2").arg(QChar(0x00ea)).arg("0") << QString("%1%2").arg(QChar(0x00ea)).arg("1")
        << QString("%1%2").arg(QChar(0x00ea)).arg("2") << QString("%1%2").arg(QChar(0x00ea)).arg("3");
    QStringList rowlistLT = QStringList() << "e0"
                                          << "e1"
                                          << "e2"
                                          << "e3";
    tbw_locted_tetrad->setHorizontalHeaderLabels(collistLT);
    tbw_locted_tetrad->setVerticalHeaderLabels(rowlistLT);
    tbw_locted_tetrad->setAlternatingRowColors(true);

    int totalHeight = tbw_locted_tetrad->horizontalHeader()->height();
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            QTableWidgetItem* item_name = new QTableWidgetItem();
            item_name->setText(QString::number(mObject.base[row][col]));
            item_name->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
            tbw_locted_tetrad->setItem(row, col, item_name);
            // tbw_locted_tetrad->setColumnWidth(col, 90);
        }
        totalHeight += tbw_locted_tetrad->verticalHeader()->sectionSize(row);
    }
    tbw_locted_tetrad->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tbw_locted_tetrad->setMinimumHeight(totalHeight);
    tbw_locted_tetrad->setMaximumHeight(totalHeight);

    QHeaderView* header = tbw_locted_tetrad->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Stretch);

    pub_locted_reset = new QPushButton("reset");
    pub_locted_set = new QPushButton("set");
    pub_locted_ortho = new QPushButton("orthonorm");

    cob_locted_predef = new QComboBox();
    for (int i = 0; i < m4d::NUM_PREDEF_TETRADS; i++) {
        cob_locted_predef->insertItem(i, m4d::stl_predef_tetrads[i]);
    }

    QGridLayout* layout_locted_tetrads = new QGridLayout();
    layout_locted_tetrads->addWidget(tbw_locted_tetrad, 0, 0, 1, 4);
    layout_locted_tetrads->addWidget(pub_locted_reset, 1, 0);
    layout_locted_tetrads->addWidget(pub_locted_set, 1, 1);
    layout_locted_tetrads->addWidget(pub_locted_ortho, 1, 2);
    layout_locted_tetrads->addWidget(cob_locted_predef, 1, 3);
    layout_locted_tetrads->setRowStretch(2, 10);

    wgt_locted_tetrads = new QWidget();
    wgt_locted_tetrads->setLayout(layout_locted_tetrads);

    // ----------------------
    //    tetrad boost
    // ----------------------
    lab_boost_value = new QLabel("value");
    lab_boost_step = new QLabel("step");
    lab_boost_chi = new QLabel("chi");
    led_boost_chi_value = new DoubleEdit(DEF_PREC_BOOST, 90.0, DEF_INIT_CHI_STEP);
    led_boost_chi_value->setRange(0.0, 180.0);
    led_boost_chi_step = new DoubleEdit(DEF_PREC_BOOST, DEF_INIT_CHI_STEP, DEF_INIT_CHI_STEP * 0.1);
    lab_boost_ksi = new QLabel("xi");
    led_boost_ksi_value = new DoubleEdit(DEF_PREC_BOOST, 0.0, DEF_INIT_KSI_STEP);
    led_boost_ksi_value->setRange(0.0, 360.0);
    led_boost_ksi_step = new DoubleEdit(DEF_PREC_BOOST, DEF_INIT_KSI_STEP, DEF_INIT_KSI_STEP * 0.1);
    lab_boost_vel = new QLabel("Velocity");
    led_boost_vel_value = new DoubleEdit(DEF_PREC_BOOST, 0.0, DEF_INIT_VEL_STEP);
    led_boost_vel_value->setRange(-1.0, 1.0);
    led_boost_vel_step = new DoubleEdit(DEF_PREC_BOOST, DEF_INIT_VEL_STEP, DEF_INIT_VEL_STEP * 0.1);
    led_boost_vel_step->setRange(0.0, 1.0);
    pub_boost_reset = new QPushButton("reset");

    QChar ch_chi = mGreekLetter.toChar("chi");
    if (ch_chi != QChar()) {
        lab_boost_chi->setText(ch_chi);
    }
    QChar ch_ksi = mGreekLetter.toChar("xi");
    if (ch_ksi != QChar()) {
        lab_boost_ksi->setText(ch_ksi);
    }

    QGridLayout* layout_locted_boost = new QGridLayout();
    layout_locted_boost->addWidget(lab_boost_value, 0, 1, Qt::AlignHCenter);
    layout_locted_boost->addWidget(lab_boost_step, 0, 2, Qt::AlignHCenter);
    layout_locted_boost->addWidget(lab_boost_ksi, 1, 0);
    layout_locted_boost->addWidget(led_boost_ksi_value, 1, 1);
    layout_locted_boost->addWidget(led_boost_ksi_step, 1, 2);
    layout_locted_boost->addWidget(lab_boost_chi, 2, 0);
    layout_locted_boost->addWidget(led_boost_chi_value, 2, 1);
    layout_locted_boost->addWidget(led_boost_chi_step, 2, 2);
    layout_locted_boost->addWidget(lab_boost_vel, 3, 0);
    layout_locted_boost->addWidget(led_boost_vel_value, 3, 1);
    layout_locted_boost->addWidget(led_boost_vel_step, 3, 2);
    layout_locted_boost->addWidget(pub_boost_reset, 4, 1);
    layout_locted_boost->setRowStretch(5, 10);

    wgt_locted_boost = new QWidget();
    wgt_locted_boost->setLayout(layout_locted_boost);

    // ----------------------
    //    tab
    // ----------------------
    tab_locted = new QTabWidget();
    tab_locted->addTab(wgt_locted_position, tr("Position"));
    tab_locted->addTab(wgt_locted_tetrads, tr("Tetrad Directions"));
    tab_locted->addTab(wgt_locted_boost, tr("Tetrad boost"));
}

void LoctedView::initGUI()
{
    this->setTitle("Local Tetrad");
    QGridLayout* layout_locted = new QGridLayout();
    layout_locted->addWidget(lab_locted_type, 0, 0);
    layout_locted->addWidget(cob_locted_type, 0, 1);
    layout_locted->addWidget(tab_locted, 1, 0, 1, 2);
    this->setLayout(layout_locted);
}

void LoctedView::initControl()
{
    for (int i = 0; i < 4; i++) {
        connect(lst_led_locted_pos_value[i], SIGNAL(editingFinished()), this, SLOT(slot_setPosition()));
        connect(lst_led_locted_pos_step[i], SIGNAL(textChanged(const QString)), this, SLOT(slot_setPosStep()));
        connect(lst_pub_locted_pos_decr[i], SIGNAL(pressed()), this, SLOT(slot_decrPosition()));
        connect(lst_pub_locted_pos_incr[i], SIGNAL(pressed()), this, SLOT(slot_incrPosition()));
    }
    connect(pub_pos_reset, SIGNAL(pressed()), this, SLOT(slot_resetPosition()));

    connect(cob_locted_type, SIGNAL(activated(int)), this, SLOT(slot_setLoctedType()));
    connect(pub_locted_reset, SIGNAL(pressed()), this, SLOT(slot_resetLocalTetrad()));
    connect(pub_locted_set, SIGNAL(pressed()), this, SLOT(slot_setLocalTetrad()));
    connect(pub_locted_ortho, SIGNAL(pressed()), this, SLOT(slot_orthoLocalTetrad()));
    connect(cob_locted_predef, SIGNAL(activated(int)), this, SLOT(slot_setPredefTetrad(int)));

    connect(led_boost_chi_value, SIGNAL(editingFinished()), this, SLOT(slot_setBoostParams()));
    connect(led_boost_ksi_value, SIGNAL(editingFinished()), this, SLOT(slot_setBoostParams()));
    connect(led_boost_vel_value, SIGNAL(editingFinished()), this, SLOT(slot_setBoostParams()));
    connect(led_boost_chi_step, SIGNAL(editingFinished()), this, SLOT(slot_setBoostParamSteps()));
    connect(led_boost_ksi_step, SIGNAL(editingFinished()), this, SLOT(slot_setBoostParamSteps()));
    connect(led_boost_vel_step, SIGNAL(editingFinished()), this, SLOT(slot_setBoostParamSteps()));
    connect(pub_boost_reset, SIGNAL(pressed()), this, SLOT(slot_resetBoost()));
}

void LoctedView::initStatusTips()
{
#ifdef SHOW_STATUS_TIPS
    cob_locted_type->setStatusTip(tr("Type of local tetrad."));

    pub_locted_reset->setStatusTip(tr("Reset tetrad vectors to identity."));
    pub_locted_set->setStatusTip(tr("Set the tetrad vectors."));
    pub_locted_ortho->setStatusTip(tr("Orthonormalize tetrad vectors."));
    cob_locted_predef->setStatusTip(tr("Select predefined tetrad vectors."));

    led_boost_chi_value->setStatusTip(tr("Colatitudinal angle (deg) for boost."));
    led_boost_chi_step->setStatusTip(tr("Step size for colatitudinal angle."));
    led_boost_ksi_value->setStatusTip(tr("Longitudinal angle (deg) for boost."));
    led_boost_ksi_step->setStatusTip(tr("Step size for longitudinal angle."));
    led_boost_vel_value->setStatusTip(tr("Velocity for boost."));
    led_boost_vel_step->setStatusTip(tr("Step size for velocity."));
    pub_boost_reset->setStatusTip(tr("Reset to no boost."));
#endif
}
