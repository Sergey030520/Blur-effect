#include <QApplication>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QFileDialog>
#include <QGraphicsPixmapItem>
#include <QGraphicsBlurEffect>
#include <QGraphicsScene>
#include <QPainter>

class BlurEffectWindow : public QWidget{
    Q_OBJECT
private:
    void loadImage();
public:
    BlurEffectWindow() = default;
    BlurEffectWindow(QWidget* parent);
    QImage blurImage(QImage source, int blurRadius);
};

BlurEffectWindow::BlurEffectWindow(QWidget *parent) {
    setParent(parent);
    auto* vbox = new QVBoxLayout(parent);
    auto* imgLabel = new QLabel(parent);
    imgLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    auto* colorDepthSlider = new QSlider(parent);
    colorDepthSlider->setOrientation(Qt::Horizontal);
    colorDepthSlider->setRange(0, 10);

    auto* btnOpenFile = new QPushButton("Open image", parent);

    vbox->addWidget(imgLabel);
    vbox->addWidget(colorDepthSlider);
    vbox->addWidget(btnOpenFile);

    setLayout(vbox);

    auto* imageBase = new QImage();

    connect(btnOpenFile, &QPushButton::clicked, [imgLabel, imageBase, colorDepthSlider](){
        QString sourceFile = QFileDialog::getOpenFileName(nullptr,
                                                  "Open image",
                                                  QApplication::applicationDirPath(),
                                                  "Image (*.JPG, *.jpg, *.JPEG, *.png)");
        QImage newImgLoad(sourceFile);
        if(!newImgLoad.isNull()) {
            *imageBase = newImgLoad;
            imgLabel->setPixmap(QPixmap::fromImage(*imageBase).scaled(
                    imgLabel->width(),
                    imgLabel->height(),
                    Qt::KeepAspectRatio
            ));
            colorDepthSlider->setValue(0);
        }
    });
    connect(colorDepthSlider, &QSlider::valueChanged, [imgLabel, imageBase, this](int pos){
       if(!imageBase->isNull()) {
           imgLabel->setPixmap(QPixmap::fromImage(blurImage(*imageBase, pos).scaled(
                   imgLabel->width(),
                   imgLabel->height(),
                   Qt::KeepAspectRatio
           )));
           update();
       }
    });
}

QImage BlurEffectWindow::blurImage(QImage source, int blurRadius) {
    if(source.isNull()) return QImage();
    QGraphicsScene scene;
    QGraphicsPixmapItem item;
    item.setPixmap(QPixmap::fromImage(source));

    auto *blur = new QGraphicsBlurEffect;
    blur->setBlurRadius(blurRadius);
    item.setGraphicsEffect(blur);
    scene.addItem(&item);
    QImage result(source.size(), QImage::Format_ARGB32);
    result.fill(Qt::transparent);
    QPainter painter(&result);
    scene.render(&painter, QRectF(), QRectF(0, 0, source.width(), source.height()));
    return result;
}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    BlurEffectWindow blurEffectWindow(nullptr);
    blurEffectWindow.setMinimumSize(700, 600);
    blurEffectWindow.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    blurEffectWindow.show();
    return QApplication::exec();
}

#include "main.moc"