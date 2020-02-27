#include "logic_analyzer.h"
#include "ui_logic_analyzer.h"

#include <QDebug>

using namespace adiscope;
using namespace adiscope::logic;

LogicAnalyzer::LogicAnalyzer(iio_context *ctx, adiscope::Filter *filt,
			     adiscope::ToolMenuItem *toolMenuItem,
			     QJSEngine *engine, adiscope::ToolLauncher *parent,
			     bool offline_mode_):
	Tool(ctx, toolMenuItem, nullptr, "Logic Analyzer", parent),
	ui(new Ui::LogicAnalyzer)
{
	// setup ui
	setupUi();

	// setup signals slots
	connectSignalsAndSlots();
}

LogicAnalyzer::~LogicAnalyzer()
{
	delete ui;
}

void LogicAnalyzer::on_btnChannelSettings_toggled(bool checked)
{
	triggerRightMenuToggle(
		static_cast<CustomPushButton *>(QObject::sender()), checked);
}

void LogicAnalyzer::on_btnCursors_toggled(bool checked)
{
	triggerRightMenuToggle(
		static_cast<CustomPushButton *>(QObject::sender()), checked);
}

void LogicAnalyzer::on_btnTrigger_toggled(bool checked)
{
	triggerRightMenuToggle(
		static_cast<CustomPushButton *>(QObject::sender()), checked);
}

void LogicAnalyzer::on_btnSettings_clicked(bool checked)
{
	CustomPushButton *btn = nullptr;

	if (checked && !m_menuOrder.isEmpty()) {
		btn = m_menuOrder.back();
		m_menuOrder.pop_back();
	} else {
		btn = static_cast<CustomPushButton *>(
			ui->settings_group->checkedButton());
	}

	btn->setChecked(checked);
}

void LogicAnalyzer::on_btnGeneralSettings_toggled(bool checked)
{
	triggerRightMenuToggle(
		static_cast<CustomPushButton *>(QObject::sender()), checked);
	if(checked)
		ui->btnSettings->setChecked(!checked);
}

void LogicAnalyzer::rightMenuFinished(bool opened)
{
	Q_UNUSED(opened)

	// At the end of each animation, check if there are other button check
	// actions that might have happened while animating and execute all
	// these queued actions
	while (m_menuButtonActions.size()) {
		auto pair = m_menuButtonActions.dequeue();
		toggleRightMenu(pair.first, pair.second);
	}
}

void LogicAnalyzer::setupUi()
{
	ui->setupUi(this);

	// Hide the run button
	ui->runSingleWidget->enableRunButton(false);

	int gsettings_panel = ui->stackedWidget->indexOf(ui->generalSettings);
	ui->btnGeneralSettings->setProperty("id", QVariant(-gsettings_panel));

	int measure_panel = ui->stackedWidget->insertWidget(-1, new QWidget());
	ui->btnChannelSettings->setProperty("id", QVariant(-measure_panel));

	/* Cursors Settings */
	ui->btnCursors->setProperty("id", QVariant(-1));

	/* Trigger Settings */
	int triggers_panel = ui->stackedWidget->insertWidget(-1, new QWidget());
	ui->btnTrigger->setProperty("id", QVariant(-triggers_panel));

	// default trigger menu?
	m_menuOrder.push_back(ui->btnTrigger);

	// set default menu width to 0
	ui->rightMenu->setMaximumWidth(0);
}

void LogicAnalyzer::connectSignalsAndSlots()
{
	// connect all the signals and slots here
	// between UI (View) and it's implementation (Controller)

	connect(ui->rightMenu, &MenuAnim::finished,
		this, &LogicAnalyzer::rightMenuFinished);

	connect(ui->cursorsBox, &QCheckBox::toggled, [=](bool toggled){
		if (!toggled) {
			// make sure to deselect the cursors button if
			// the cursors are disabled
			ui->btnCursors->setChecked(false);

			// we also remove the button from the history
			// so that the last menu opened button on top
			// won't open the cursors menu when it is disabled
			m_menuOrder.removeOne(ui->btnCursors);
		}
	});
}

void LogicAnalyzer::triggerRightMenuToggle(CustomPushButton *btn, bool checked)
{
	// Queue the action, if right menu animation is in progress. This way
	// the action will be remembered and performed right after the animation
	// finishes
	if (ui->rightMenu->animInProgress()) {
		m_menuButtonActions.enqueue(
			QPair<CustomPushButton *, bool>(btn, checked));
	} else {
		toggleRightMenu(btn, checked);
	}
}

void LogicAnalyzer::toggleRightMenu(CustomPushButton *btn, bool checked)
{

	qDebug() << "toggleRightMenu called!";

	int id = btn->property("id").toInt();

	if (id != -ui->stackedWidget->indexOf(ui->generalSettings)){
		if (!m_menuOrder.contains(btn)){
			m_menuOrder.push_back(btn);
		} else {
			m_menuOrder.removeOne(btn);
			m_menuOrder.push_back(btn);
		}
	}

	if (checked) {
		settingsPanelUpdate(id);
	}

	ui->rightMenu->toggleMenu(checked);
}

void LogicAnalyzer::settingsPanelUpdate(int id)
{
	if (id >= 0) {
		ui->stackedWidget->setCurrentIndex(0);
	} else {
		ui->stackedWidget->setCurrentIndex(-id);
	}

	for (int i = 0; i < ui->stackedWidget->count(); i++) {
		QSizePolicy::Policy policy = QSizePolicy::Ignored;

		if (i == ui->stackedWidget->currentIndex()) {
			policy = QSizePolicy::Expanding;
		}
		QWidget *widget = ui->stackedWidget->widget(i);
		widget->setSizePolicy(policy, policy);
	}
	ui->stackedWidget->adjustSize();
}
