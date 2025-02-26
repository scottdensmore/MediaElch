#include "ui/settings/TvShowSettingsWidget.h"
#include "ui_TvShowSettingsWidget.h"

#include "settings/DataFile.h"
#include "settings/Settings.h"

#include <QLineEdit>

TvShowSettingsWidget::TvShowSettingsWidget(QWidget* parent) : QWidget(parent), ui(new Ui::TvShowSettingsWidget)
{
    ui->setupUi(this);

#ifdef Q_OS_MAC
    QFont smallFont = ui->label_46->font();
    smallFont.setPointSize(smallFont.pointSize() - 1);

    ui->label_46->setFont(smallFont);
    ui->label_47->setFont(smallFont);
#endif

    ui->showBackdrop->setProperty("dataFileType", static_cast<int>(DataFileType::TvShowBackdrop));
    ui->showBanner->setProperty("dataFileType", static_cast<int>(DataFileType::TvShowBanner));
    ui->showCharacterArt->setProperty("dataFileType", static_cast<int>(DataFileType::TvShowCharacterArt));
    ui->showClearArt->setProperty("dataFileType", static_cast<int>(DataFileType::TvShowClearArt));
    ui->showEpisodeNfo->setProperty("dataFileType", static_cast<int>(DataFileType::TvShowEpisodeNfo));
    ui->showEpisodeThumbnail->setProperty("dataFileType", static_cast<int>(DataFileType::TvShowEpisodeThumb));
    ui->showLogo->setProperty("dataFileType", static_cast<int>(DataFileType::TvShowLogo));
    ui->showThumb->setProperty("dataFileType", static_cast<int>(DataFileType::TvShowThumb));
    ui->showNfo->setProperty("dataFileType", static_cast<int>(DataFileType::TvShowNfo));
    ui->showPoster->setProperty("dataFileType", static_cast<int>(DataFileType::TvShowPoster));
    ui->showSeasonBackdrop->setProperty("dataFileType", static_cast<int>(DataFileType::TvShowSeasonBackdrop));
    ui->showSeasonBanner->setProperty("dataFileType", static_cast<int>(DataFileType::TvShowSeasonBanner));
    ui->showSeasonPoster->setProperty("dataFileType", static_cast<int>(DataFileType::TvShowSeasonPoster));
    ui->showSeasonThumb->setProperty("dataFileType", static_cast<int>(DataFileType::TvShowSeasonThumb));

    for (auto* lineEdit : findChildren<PlaceholderLineEdit*>()) {
        lineEdit->setPlaceholders({"baseFileName"});
    }
    ui->showSeasonBackdrop->setPlaceholders({"baseFileName", "seasonNumber"});
    ui->showSeasonBanner->setPlaceholders({"baseFileName", "seasonNumber"});
    ui->showSeasonPoster->setPlaceholders({"baseFileName", "seasonNumber"});
    ui->showSeasonThumb->setPlaceholders({"baseFileName", "seasonNumber"});
}

TvShowSettingsWidget::~TvShowSettingsWidget()
{
    delete ui;
}

void TvShowSettingsWidget::setSettings(Settings& settings)
{
    m_settings = &settings;
}

void TvShowSettingsWidget::loadSettings()
{
    const auto loadLineEdit = [this](auto* lineEdit) {
        if (lineEdit->property("dataFileType").isNull()) {
            return;
        }
        DataFileType dataFileType = DataFileType(lineEdit->property("dataFileType").toInt());
        QVector<DataFile> dataFiles = m_settings->dataFiles(dataFileType);
        QStringList filenames;
        for (const DataFile& dataFile : dataFiles) {
            filenames << dataFile.fileName();
        }
        lineEdit->setText(filenames.join(","));
    };
    for (auto* lineEdit : findChildren<QLineEdit*>()) {
        loadLineEdit(lineEdit);
    }
    for (auto* lineEdit : findChildren<PlaceholderLineEdit*>()) {
        loadLineEdit(lineEdit);
    }
}

void TvShowSettingsWidget::saveSettings()
{
    QVector<DataFile> dataFiles;
    const auto storeLineEdit = [&dataFiles](auto* lineEdit) {
        if (lineEdit->property("dataFileType").isNull()) {
            return;
        }
        int pos = 0;
        DataFileType dataFileType = DataFileType(lineEdit->property("dataFileType").toInt());
        QStringList filenames = lineEdit->text().split(",", ElchSplitBehavior::SkipEmptyParts);
        for (const QString& filename : filenames) {
            DataFile df(dataFileType, filename.trimmed(), pos++);
            dataFiles << df;
        }
    };
    for (auto* lineEdit : findChildren<QLineEdit*>()) {
        storeLineEdit(lineEdit);
    }
    for (auto* lineEdit : findChildren<PlaceholderLineEdit*>()) {
        storeLineEdit(lineEdit);
    }
}
