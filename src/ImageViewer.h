#pragma once

#include <QtWidgets/QMainWindow>
#include <QtWidgets>
#include <ui_ImageViewer.h>
#include "ViewerWidget.h"
#include "NewImageDialog.h"

class ImageViewer : public QMainWindow
{
	Q_OBJECT

public:
	ImageViewer(QWidget* parent = Q_NULLPTR);

private:
	Ui::ImageViewerClass* ui;
	NewImageDialog* newImgDialog;

	QSettings settings;
	QMessageBox msgBox;


	// cv3 stuff
	QColor currentPenColor;
	QColor currentFillColor;
	QVector<QPoint> geometryPoints;
	QPoint mousePosition[2];

	bool drawingEnabled = false;

	void infoMessage(QString message);
	void warningMessage(QString message);
	void printPoints(QVector<QPoint> geometryPoints);


	//ViewerWidget functions
	ViewerWidget* getViewerWidget(int tabId);
	ViewerWidget* getCurrentViewerWidget();

	//Event filters
	bool eventFilter(QObject* obj, QEvent* event);

	//ViewerWidget Events
	bool ViewerWidgetEventFilter(QObject* obj, QEvent* event);
	void ViewerWidgetMouseButtonPress(ViewerWidget* w, QEvent* event);
	void ViewerWidgetMouseButtonRelease(ViewerWidget* w, QEvent* event);
	void ViewerWidgetMouseMove(ViewerWidget* w, QEvent* event);
	void ViewerWidgetLeave(ViewerWidget* w, QEvent* event);
	void ViewerWidgetEnter(ViewerWidget* w, QEvent* event);
	void ViewerWidgetWheel(ViewerWidget* w, QEvent* event);

	//ImageViewer Events
	void closeEvent(QCloseEvent* event);

	//Image functions
	void openNewTabForImg(ViewerWidget* vW);
	bool openImage(QString filename);
	bool saveImage(QString filename);
	void clearImage();
	void setBackgroundColor(QColor color);

	//Inline functions
	inline bool isImgOpened() { return ui->tabWidget->count() == 0 ? false : true; }

private slots:
	//Tabs slots
	void on_tabWidget_tabCloseRequested(int tabId);
	void on_actionRename_triggered();

	//Image slots
	void on_actionNew_triggered();
	void newImageAccepted();
	void on_actionOpen_triggered();
	void on_actionSave_as_triggered();
	void on_actionClear_triggered();
	void on_actionSet_background_color_triggered();

	// cv5 slots
	void on_pushButton_PenColorDialog_clicked();
	void on_pushButton_FillColorDialog_clicked();

	void on_pushButton_CreateGeometry_clicked();
	void on_pushButton_ClearGeometry_clicked();

	void on_pushButton_Rotate_clicked();
	void on_pushButton_Shear_clicked();
	void on_pushButton_Symmetry_clicked();

	void on_comboBox_InterpolationMethod_currentIndexChanged(int index);
};
