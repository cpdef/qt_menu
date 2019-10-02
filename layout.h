#pragma once

#include <QWidget>
#include <QPushButton>
#include <QTableWidget>
#include <QLineEdit>
#include <QTabWidget>
#include <QGridLayout>
#include <QString>
#include <QKeyEvent>

struct desktopEntry_t {
    QString name;
    QString icon;
    QString exec;
};

typedef struct desktopEntry_t desktopEntry;

class MainLayout : public QWidget {

    Q_OBJECT

    public:
        MainLayout(QWidget *parent = 0);
    private:
        QGridLayout *glayout;
        std::vector<QPushButton *> buttons;
        QLineEdit *le;
        std::vector<desktopEntry> applications;
    public slots:
        void search(const QString& text);
        void exec();
        virtual void keyPressEvent(QKeyEvent *event);
};

