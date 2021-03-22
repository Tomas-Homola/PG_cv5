#include "ImageViewer.h"

// custom farby: cervena: #ED1C24; zelena: #00AD33; modra: #1F75FE

ImageViewer::ImageViewer(QWidget* parent)
	: QMainWindow(parent), ui(new Ui::ImageViewerClass)
{
	ui->setupUi(this);

	currentPenColor = QColor("#000000");
	currentFillColor = QColor("#1F75FE"); // custom modra farba
	ui->pushButton_PenColorDialog->setStyleSheet("background-color:#000000");
	ui->pushButton_FillColorDialog->setStyleSheet("background-color:#1F75FE");

	openNewTabForImg(new ViewerWidget("Default window", QSize(800, 450)));
	ui->tabWidget->setCurrentIndex(ui->tabWidget->count() - 1);

	ui->pushButton_ClearGeometry->setEnabled(false);
	ui->groupBox_Transformations->setEnabled(false);
	ui->comboBox_InterpolationMethod->setEnabled(false);
}

void ImageViewer::infoMessage(QString message)
{
	msgBox.setWindowTitle("Info message");
	msgBox.setIcon(QMessageBox::Information);
	msgBox.setText(message);
	msgBox.exec();
}
void ImageViewer::warningMessage(QString message)
{
	msgBox.setWindowTitle("Warning message");
	msgBox.setIcon(QMessageBox::Warning);
	msgBox.setText(message);
	msgBox.exec();
}

void ImageViewer::printPoints(QVector<QPoint> geometryPoints)
{
	for (int i = 0; i < geometryPoints.size(); i++)
		qDebug() << geometryPoints.at(i);
	qDebug() << "\n";
}

//ViewerWidget functions
ViewerWidget* ImageViewer::getViewerWidget(int tabId)
{
	QScrollArea* s = static_cast<QScrollArea*>(ui->tabWidget->widget(tabId));
	if (s) {
		ViewerWidget* vW = static_cast<ViewerWidget*>(s->widget());
		return vW;
	}
	return nullptr;
}
ViewerWidget* ImageViewer::getCurrentViewerWidget()
{
	return getViewerWidget(ui->tabWidget->currentIndex());
}

// Event filters
bool ImageViewer::eventFilter(QObject* obj, QEvent* event)
{
	if (obj->objectName() == "ViewerWidget") {
		return ViewerWidgetEventFilter(obj, event);
	}
	return false;
}

//ViewerWidget Events
bool ImageViewer::ViewerWidgetEventFilter(QObject* obj, QEvent* event)
{
	ViewerWidget* w = static_cast<ViewerWidget*>(obj);

	if (!w) {
		return false;
	}

	if (event->type() == QEvent::MouseButtonPress) {
		ViewerWidgetMouseButtonPress(w, event);
	}
	else if (event->type() == QEvent::MouseButtonRelease) {
		ViewerWidgetMouseButtonRelease(w, event);
	}
	else if (event->type() == QEvent::MouseMove) {
		ViewerWidgetMouseMove(w, event);
	}
	else if (event->type() == QEvent::Leave) {
		ViewerWidgetLeave(w, event);
	}
	else if (event->type() == QEvent::Enter) {
		ViewerWidgetEnter(w, event);
	}
	else if (event->type() == QEvent::Wheel) {
		ViewerWidgetWheel(w, event);
	}

	return QObject::eventFilter(obj, event);
}
void ImageViewer::ViewerWidgetMouseButtonPress(ViewerWidget* w, QEvent* event)
{
	QMouseEvent* e = static_cast<QMouseEvent*>(event);
	if (drawingEnabled) // ide sa kreslit
	{
		if (e->button() == Qt::LeftButton)
		{
			geometryPoints.push_back(e->pos());
			if (geometryPoints.size() > 1)
			{
				getCurrentViewerWidget()->createLineWithAlgorithm(geometryPoints.at(geometryPoints.size() - 1), geometryPoints.at(geometryPoints.size() - 2), currentPenColor, ui->comboBox_SelectAlgorithm->currentIndex());
			}
			//printPoints(geometryPoints);

		}
		else if (e->button() == Qt::RightButton) // ukoncenie kreslenia
		{
			if (geometryPoints.size() == 1) // kliknutie pravym hned po zadani prveho bodu
			{
				geometryPoints.push_back(e->pos());
				getCurrentViewerWidget()->createLineWithAlgorithm(geometryPoints.at(1), geometryPoints.at(0), currentPenColor, ui->comboBox_SelectAlgorithm->currentIndex());
			}
			else if (geometryPoints.size() > 2) // ak by uz bola nakreslena usecka, tak sa znovu nenakresli
			{
				getCurrentViewerWidget()->createLineWithAlgorithm(geometryPoints.at(geometryPoints.size() - 1), geometryPoints.at(0), currentPenColor, ui->comboBox_SelectAlgorithm->currentIndex());
			}
			
			drawingEnabled = false;
			ui->groupBox_Transformations->setEnabled(true);

			if (geometryPoints.size() == 3)
				ui->comboBox_InterpolationMethod->setEnabled(true);

			getCurrentViewerWidget()->clear();
			getCurrentViewerWidget()->createGeometry(geometryPoints, currentPenColor, currentFillColor, ui->comboBox_SelectAlgorithm->currentIndex(), ui->comboBox_InterpolationMethod->currentIndex());
			//printPoints(geometryPoints);
		}
	}
	else // nejde sa kreslit, ale posuvat polygon
	{
		mousePosition[0] = e->pos();
	}
		
}
void ImageViewer::ViewerWidgetMouseButtonRelease(ViewerWidget* w, QEvent* event)
{
	QMouseEvent* e = static_cast<QMouseEvent*>(event);

	if (e->button() == Qt::LeftButton && !drawingEnabled)
	{
		if (mousePosition[1] != e->pos())
		{
			mousePosition[1] = e->pos();

			int pX = mousePosition[1].x() - mousePosition[0].x();
			int pY = mousePosition[1].y() - mousePosition[0].y();

			for (int i = 0; i < geometryPoints.size(); i++) // prepocitanie suradnic polygonu
			{
				// poznamka pre autora: [i] vracia modifikovatelny objekt, .at(i) vracia const objekt
				geometryPoints[i].setX(geometryPoints[i].x() + pX);
				geometryPoints[i].setY(geometryPoints[i].y() + pY);
			}

			getCurrentViewerWidget()->clear(); // vymazanie stareho polygonu
			getCurrentViewerWidget()->createGeometry(geometryPoints, currentPenColor, currentFillColor, ui->comboBox_SelectAlgorithm->currentIndex(), ui->comboBox_InterpolationMethod->currentIndex());
		}
		
	}
}
void ImageViewer::ViewerWidgetMouseMove(ViewerWidget* w, QEvent* event)
{
	QMouseEvent* e = static_cast<QMouseEvent*>(event);

	if (e->buttons() == Qt::LeftButton && !drawingEnabled)
	{
		mousePosition[1] = e->pos();
		int pX = mousePosition[1].x() - mousePosition[0].x();
		int pY = mousePosition[1].y() - mousePosition[0].y();
		
		for (int i = 0; i < geometryPoints.size(); i++)
		{
			geometryPoints[i].setX(geometryPoints[i].x() + pX);
			geometryPoints[i].setY(geometryPoints[i].y() + pY);
		}

		getCurrentViewerWidget()->clear(); // vymazanie stareho polygonu
		getCurrentViewerWidget()->createGeometry(geometryPoints, currentPenColor, currentFillColor, ui->comboBox_SelectAlgorithm->currentIndex(), ui->comboBox_InterpolationMethod->currentIndex());

		mousePosition[0] = mousePosition[1];
	}
}
void ImageViewer::ViewerWidgetLeave(ViewerWidget* w, QEvent* event)
{
}
void ImageViewer::ViewerWidgetEnter(ViewerWidget* w, QEvent* event)
{
}
void ImageViewer::ViewerWidgetWheel(ViewerWidget* w, QEvent* event)
{
	QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);

	if (!drawingEnabled)
	{
		double scaleFactorXY = 0.0;
		double sX = geometryPoints.at(0).x();
		double sY = geometryPoints.at(0).y();


		if (wheelEvent->angleDelta().y() > 0)
			scaleFactorXY = 1.25;
		else if (wheelEvent->angleDelta().y() < 0)
			scaleFactorXY = 0.75;

		for (int i = 0; i < geometryPoints.size(); i++)
		{
			geometryPoints[i].setX(sX + static_cast<int>((geometryPoints.at(i).x() - sX) * scaleFactorXY));
			geometryPoints[i].setY(sY + static_cast<int>((geometryPoints.at(i).y() - sY) * scaleFactorXY));
		}

		getCurrentViewerWidget()->clear();
		getCurrentViewerWidget()->createGeometry(geometryPoints, currentPenColor, currentFillColor, ui->comboBox_SelectAlgorithm->currentIndex(), ui->comboBox_InterpolationMethod->currentIndex());
	}
}

//ImageViewer Events
void ImageViewer::closeEvent(QCloseEvent* event)
{
	if (QMessageBox::Yes == QMessageBox::question(this, "Close Confirmation", "Are you sure you want to exit?", QMessageBox::Yes | QMessageBox::No))
	{
		event->accept();
	}
	else {
		event->ignore();
	}
}

//Image functions
void ImageViewer::openNewTabForImg(ViewerWidget* vW)
{
	QScrollArea* scrollArea = new QScrollArea;
	scrollArea->setWidget(vW);

	scrollArea->setBackgroundRole(QPalette::Dark);
	scrollArea->setWidgetResizable(true);
	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

	vW->setObjectName("ViewerWidget");
	vW->installEventFilter(this);

	QString name = vW->getName();

	ui->tabWidget->addTab(scrollArea, name);
}
bool ImageViewer::openImage(QString filename)
{
	QFileInfo fi(filename);

	QString name = fi.baseName();
	openNewTabForImg(new ViewerWidget(name, QSize(0, 0)));
	ui->tabWidget->setCurrentIndex(ui->tabWidget->count() - 1);

	ViewerWidget* w = getCurrentViewerWidget();

	QImage loadedImg(filename);
	return w->setImage(loadedImg);
}
bool ImageViewer::saveImage(QString filename)
{
	QFileInfo fi(filename);
	QString extension = fi.completeSuffix();
	ViewerWidget* w = getCurrentViewerWidget();

	QImage* img = w->getImage();
	return img->save(filename, extension.toStdString().c_str());
}
void ImageViewer::clearImage()
{
	ViewerWidget* w = getCurrentViewerWidget();
	w->clear();
}
void ImageViewer::setBackgroundColor(QColor color)
{
	ViewerWidget* w = getCurrentViewerWidget();
	if (w != nullptr)
		w->clear(color);
	else
		warningMessage("No image opened");
}

//Slots

//Tabs slots
void ImageViewer::on_tabWidget_tabCloseRequested(int tabId)
{
	ViewerWidget* vW = getViewerWidget(tabId);
	delete vW; //vW->~ViewerWidget();
	ui->tabWidget->removeTab(tabId);
}
void ImageViewer::on_actionRename_triggered()
{
	if (!isImgOpened()) {
		msgBox.setText("No image is opened.");
		msgBox.setIcon(QMessageBox::Information);
		msgBox.exec();
		return;
	}
	ViewerWidget* w = getCurrentViewerWidget();
	bool ok;
	QString text = QInputDialog::getText(this, QString("Rename image"), tr("Image name:"), QLineEdit::Normal, w->getName(), &ok);
	if (ok && !text.trimmed().isEmpty())
	{
		w->setName(text);
		ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), text);
	}
}

//Image slots
void ImageViewer::on_actionNew_triggered()
{
	newImgDialog = new NewImageDialog(this);
	connect(newImgDialog, SIGNAL(accepted()), this, SLOT(newImageAccepted()));
	newImgDialog->exec();
}
void ImageViewer::newImageAccepted()
{
	NewImageDialog* newImgDialog = static_cast<NewImageDialog*>(sender());

	int width = newImgDialog->getWidth();
	int height = newImgDialog->getHeight();
	QString name = newImgDialog->getName();
	openNewTabForImg(new ViewerWidget(name, QSize(width, height)));
	ui->tabWidget->setCurrentIndex(ui->tabWidget->count() - 1);
}
void ImageViewer::on_actionOpen_triggered()
{
	QString folder = settings.value("folder_img_load_path", "").toString();

	QString fileFilter = "Image data (*.bmp *.gif *.jpg *.jpeg *.png *.pbm *.pgm *.ppm .*xbm .* xpm);;All files (*)";
	QString fileName = QFileDialog::getOpenFileName(this, "Load image", folder, fileFilter);
	if (fileName.isEmpty()) { return; }

	QFileInfo fi(fileName);
	settings.setValue("folder_img_load_path", fi.absoluteDir().absolutePath());

	if (!openImage(fileName)) {
		msgBox.setText("Unable to open image.");
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.exec();
	}
}
void ImageViewer::on_actionSave_as_triggered()
{
	if (!isImgOpened()) {
		msgBox.setText("No image to save.");
		msgBox.setIcon(QMessageBox::Information);
		msgBox.exec();
		return;
	}
	QString folder = settings.value("folder_img_save_path", "").toString();

	ViewerWidget* w = getCurrentViewerWidget();

	QString fileFilter = "Image data (*.bmp *.gif *.jpg *.jpeg *.png *.pbm *.pgm *.ppm .*xbm .* xpm);;All files (*)";
	QString fileName = QFileDialog::getSaveFileName(this, "Save image", folder + "/" + w->getName(), fileFilter);
	if (fileName.isEmpty()) { return; }

	QFileInfo fi(fileName);
	settings.setValue("folder_img_save_path", fi.absoluteDir().absolutePath());

	if (!saveImage(fileName)) {
		msgBox.setText("Unable to save image.");
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.exec();
	}
	else {
		msgBox.setText(QString("File %1 saved.").arg(fileName));
		msgBox.setIcon(QMessageBox::Information);
		msgBox.exec();
	}
}
void ImageViewer::on_actionClear_triggered()
{
	if (!isImgOpened()) {
		msgBox.setText("No image is opened.");
		msgBox.setIcon(QMessageBox::Information);
		msgBox.exec();
		return;
	}
	clearImage();
}
void ImageViewer::on_actionSet_background_color_triggered()
{
	QColor backgroundColor = QColorDialog::getColor(Qt::white, this, "Select color of background");
	if (backgroundColor.isValid()) {
		setBackgroundColor(backgroundColor);
	}
}

void ImageViewer::on_pushButton_PenColorDialog_clicked()
{
	QColor chosenColor = QColorDialog::getColor(currentPenColor.name(), this, "Select pen color");

	if (chosenColor.isValid())
	{
		currentPenColor = chosenColor;
		ui->pushButton_PenColorDialog->setStyleSheet(QString("background-color:%1").arg(chosenColor.name()));
		
		getCurrentViewerWidget()->createGeometry(geometryPoints, currentPenColor, currentFillColor, ui->comboBox_SelectAlgorithm->currentIndex(), ui->comboBox_InterpolationMethod->currentIndex());
	}
}

void ImageViewer::on_pushButton_FillColorDialog_clicked()
{
	QColor chosenColor = QColorDialog::getColor(currentPenColor.name(), this, "Select fill color");

	if (chosenColor.isValid())
	{
		currentFillColor = chosenColor;
		ui->pushButton_FillColorDialog->setStyleSheet(QString("background-color:%1").arg(chosenColor.name()));

		getCurrentViewerWidget()->createGeometry(geometryPoints, currentPenColor, currentFillColor, ui->comboBox_SelectAlgorithm->currentIndex(), ui->comboBox_InterpolationMethod->currentIndex());
	}
}

void ImageViewer::on_pushButton_CreateGeometry_clicked()
{
	ui->pushButton_CreateGeometry->setEnabled(false);
	ui->pushButton_ClearGeometry->setEnabled(true);

	drawingEnabled = true;
}

void ImageViewer::on_pushButton_ClearGeometry_clicked()
{
	ui->pushButton_ClearGeometry->setEnabled(false);
	ui->pushButton_CreateGeometry->setEnabled(true);
	ui->groupBox_Transformations->setEnabled(false);
	ui->comboBox_InterpolationMethod->setEnabled(false);

	drawingEnabled = false;
	geometryPoints.clear();

	getCurrentViewerWidget()->clear();
}

void ImageViewer::on_pushButton_Rotate_clicked()
{
	double angle = (ui->spinBox_Angle->value() / 180.0) * M_PI;
	double sX = geometryPoints.at(0).x();
	double sY = geometryPoints.at(0).y();
	double x = 0.0, y = 0.0;

	if (ui->spinBox_Angle->value() < 0)
	{
		//qDebug() << "clockwise";
		
		for (int i = 1; i < geometryPoints.size(); i++)
		{
			x = geometryPoints.at(i).x();
			y = geometryPoints.at(i).y();

			geometryPoints[i].setX(static_cast<int>((x - sX) * qCos(angle) + (y - sY) * qSin(angle) + sX));
			geometryPoints[i].setY(static_cast<int>(-(x - sX) * qSin(angle) + (y - sY) * qCos(angle) + sY));
		}
	}
	else if (ui->spinBox_Angle->value() > 0)
	{
		//qDebug() << "anti-clockwise";
		angle = 2 * M_PI - angle;
		for (int i = 1; i < geometryPoints.size(); i++)
		{
			x = geometryPoints.at(i).x();
			y = geometryPoints.at(i).y();

			geometryPoints[i].setX(static_cast<int>((x - sX) * qCos(angle) - (y - sY) * qSin(angle) + sX));
			geometryPoints[i].setY(static_cast<int>((x - sX) * qSin(angle) + (y - sY) * qCos(angle) + sY));
		}
	}

	getCurrentViewerWidget()->clear();
	getCurrentViewerWidget()->createGeometry(geometryPoints, currentPenColor, currentFillColor, ui->comboBox_SelectAlgorithm->currentIndex(), ui->comboBox_InterpolationMethod->currentIndex());
}

void ImageViewer::on_pushButton_Shear_clicked()
{
	double shearFactor = ui->doubleSpinBox_ShearFactor->value();
	double sY = geometryPoints.at(0).y();

	for (int i = 1; i < geometryPoints.size(); i++)
		geometryPoints[i].setX(static_cast<int>(geometryPoints.at(i).x() + shearFactor * (geometryPoints.at(i).y() - sY)));

	getCurrentViewerWidget()->clear();
	getCurrentViewerWidget()->createGeometry(geometryPoints, currentPenColor, currentFillColor, ui->comboBox_SelectAlgorithm->currentIndex(), ui->comboBox_InterpolationMethod->currentIndex());
}

void ImageViewer::on_pushButton_Symmetry_clicked()
{
	// symetria polygonu cez usecku medzi prvym a druhym bodom
	// symetria usecky cez horizontalnu priamku prechadzajucu stredom usecky
	double u = static_cast<double>(geometryPoints.at(1).x()) - geometryPoints.at(0).x();
	double v = static_cast<double>(geometryPoints.at(1).y()) - geometryPoints.at(0).y();
	double a = v;
	double b = -u;
	double c = -a * geometryPoints.at(0).x() - b * geometryPoints.at(0).y();
	double x = 0.0, y = 0.0;
	int midPointX = qAbs((geometryPoints.at(1).x() + geometryPoints.at(0).x()) / 2);
	int midPointY = qAbs((geometryPoints.at(1).y() + geometryPoints.at(0).y()) / 2);
	int deltaY = 0;

	if (geometryPoints.size() == 2) // usecka
	{
		deltaY = qAbs(geometryPoints.at(0).y() - midPointY); // 

		if (geometryPoints.at(0).y() < midPointY)
		{
			geometryPoints[0].setY(geometryPoints.at(0).y() + 2 * deltaY);
			geometryPoints[1].setY(geometryPoints.at(1).y() - 2 * deltaY);
		}
		else if (geometryPoints.at(0).y() > midPointY)
		{
			geometryPoints[0].setY(geometryPoints.at(0).y() - 2 * deltaY);
			geometryPoints[1].setY(geometryPoints.at(1).y() + 2 * deltaY);
		}
	}
	else if (geometryPoints.size() > 2) // polygon
	{
		for (int i = 2; i < geometryPoints.size(); i++)
		{
			x = geometryPoints.at(i).x();
			y = geometryPoints.at(i).y();

			geometryPoints[i].setX(static_cast<int>(x - 2 * a * ((a * x + b * y + c) / (a * a + b * b))));
			geometryPoints[i].setY(static_cast<int>(y - 2 * b * ((a * x + b * y + c) / (a * a + b * b))));
		}
	}

	getCurrentViewerWidget()->clear();
	getCurrentViewerWidget()->createGeometry(geometryPoints, currentPenColor, currentFillColor, ui->comboBox_SelectAlgorithm->currentIndex(), ui->comboBox_InterpolationMethod->currentIndex());
}

void ImageViewer::on_comboBox_InterpolationMethod_currentIndexChanged(int index)
{
	getCurrentViewerWidget()->clear();
	getCurrentViewerWidget()->createGeometry(geometryPoints, currentPenColor, currentFillColor, ui->comboBox_SelectAlgorithm->currentIndex(), ui->comboBox_InterpolationMethod->currentIndex());
}

