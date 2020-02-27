#ifndef LOGIC_ANALYZER_H
#define LOGIC_ANALYZER_H

#include <QList>
#include <QQueue>

#include "tool.hpp"

namespace Ui {
class LogicAnalyzer;
}
namespace adiscope {

class Filter;

namespace logic {

class LogicAnalyzer : public Tool {
	Q_OBJECT
public:
	explicit LogicAnalyzer(struct iio_context *ctx, Filter *filt,
			 ToolMenuItem *toolMenuItem, QJSEngine *engine,
			 ToolLauncher *parent, bool offline_mode_ = 0);
	~LogicAnalyzer();

private Q_SLOTS:

	void on_btnChannelSettings_toggled(bool);
	void on_btnCursors_toggled(bool);
	void on_btnTrigger_toggled(bool);

	void on_btnSettings_clicked(bool checked);
	void on_btnGeneralSettings_toggled(bool);
	void rightMenuFinished(bool opened);

private:
	void setupUi();
	void connectSignalsAndSlots();
	void triggerRightMenuToggle(CustomPushButton *, bool checked);
	void toggleRightMenu(CustomPushButton *, bool checked);
	void settingsPanelUpdate(int id);

private:
	Ui::LogicAnalyzer *ui;

	QList<CustomPushButton *> m_menuOrder;
	QQueue<QPair<CustomPushButton *, bool>> m_menuButtonActions;
};
} // namespace logic
} // namespace adiscope

#endif // LOGIC_ANALYZER_H
