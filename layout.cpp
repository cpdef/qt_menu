#include "layout.h"

#include <QtCore/QDebug>
#include <QtGui/QWindow>
#include <QtGui/QScreen>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGraphicsOpacityEffect>
#include <QSizePolicy>
#include <QDir>
#include <QPixmap>
#include <QIcon>
#include <QApplication>
#include <QRegularExpression>

#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <stdlib.h>

MainLayout::MainLayout(QWidget *parent) 
{

    QVBoxLayout *vbox = new QVBoxLayout(this);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    glayout = new QGridLayout();
    
    le = new QLineEdit("");
    le->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    vbox->addWidget(le, 1);
    vbox->addLayout(glayout, 20);

    handleResize();

    setLayout(vbox);
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    QGraphicsOpacityEffect *op = new QGraphicsOpacityEffect(this);
    op->setOpacity(0.9);
    setGraphicsEffect(op);
    setAutoFillBackground(1);

    connect(le, SIGNAL(textChanged(const QString&)), this, SLOT(search(const QString&)));
    connect(le, SIGNAL(returnPressed()), this, SLOT(exec()));
}

void MainLayout::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    qDebug() << "showEvent" << windowHandle();
    auto *window = windowHandle();
    if (!window)
    {
        return;
    }
    connect(window, &QWindow::screenChanged, this, &MainLayout::handleResize);
    handleResize();
}

void MainLayout::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    handleResize();
}

void MainLayout::handleResize() 
{
    auto *window = windowHandle();
    if (!window)
    {
        return;
    }

    auto *screen = window->screen();
    if (!screen)
    {
        return;
    }

    auto r = screen->geometry();
    qDebug() << "forcing max size on screen" << screen->name();
    setFixedSize(r.width()*0.9, r.height()*0.9);

    int w = width();//glayout->geometry().width();
    int h = height();//glayout->geometry().height();

    glayout->setSpacing(w/30);

    int cols = 7;
    int rows = h/(w/7);

    // If there are buttons already in the layout, remove them
    for (auto button : buttons)
    {
        glayout->removeWidget(button);
    }

    // Add more buttons, if needed
    int numButtons = rows * cols;
    while (buttons.size() < numButtons)
    {
        QPushButton *button = new QPushButton(this);
        buttons.push_back(button);

        // size policy
        button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        QSizePolicy sp = button->sizePolicy();
        sp.setRetainSizeWhenHidden(true);
        button->setSizePolicy(sp);

        button->setVisible(false);
        connect(button, SIGNAL(clicked()), this, SLOT(exec()));
    }

    for (int r =0;r<rows;r++)
    {
        for (int c = 0;c<cols;c++)
        {
            glayout->addWidget(buttons[c+r*cols], r, c);
        }
    }

    while (buttons.size() > numButtons)
    {
        delete buttons.back();
        buttons.pop_back();
    }
}

void MainLayout::search(const QString& text)
{
    QDir application_dir("/usr/share/applications");
    QStringList entrys = application_dir.entryList();
    QString filename;
    std::ifstream dentry_file;
    std::string line;
    desktopEntry dentry;
    size_t index;
    float score;
    typedef std::pair<desktopEntry, float> d_pair;
    std::vector<d_pair> app_rank_pair_vector;
    bool status;

    applications.clear();
    for (int i = 0;i<entrys.size();i++)
    {
        filename = QString("/usr/share/applications/")+entrys[i];
        dentry_file = std::ifstream(filename.toStdString());

        if (dentry_file.is_open())
        {
            dentry.name = QString("");
            dentry.icon = QString("");
            dentry.exec = QString("");
            
            // read file
            while ( getline (dentry_file,line) )
            {
                size_t index = line.find("=");
                if (index == -1)
                    continue;
                std::string key = line.substr(0,index);
                std::string value = line.substr(index+1, line.size()-index-1);
                if (key == "Name")
                        dentry.name = QString::fromUtf8(value.c_str());
                if (key == "Exec")
                {
                        dentry.exec = QString::fromUtf8(value.c_str());
                        dentry.exec.replace(QRegularExpression("\%[[:print:]]"), "");
                }
                if (key == "Icon")
                        dentry.icon = QString::fromUtf8(value.c_str());
            }
            dentry_file.close();

            // find text in filename/dentry
            // calculate a score/ranking
            score = 0;
            index = dentry.name.toLower().indexOf(text.toLower());
            if ((index != -1))
            {
                score += 1.0/(index+1);
            }
            
            index = entrys[i].indexOf(text.toLower());
            if ((index != -1) && (index < entrys[i].indexOf(QString("."))))
            {
                score += 1.0/(index+7); // 6 = negative priority
            }
           
            index = dentry.exec.indexOf(text.toLower());
            if ((index != -1))
            {
                score += 1.0/(index+11);
            }

            index = dentry.icon.indexOf(text.toLower());
            if ((index != -1))
            {
                score += 1.0/(index+9);
            }
            if (text == QString(""))
                score = 1;
            if (score == 0)
                continue;

            app_rank_pair_vector.push_back(d_pair(dentry, score));
        }
    }
    
    // we got all the entrys so now sort them
    std::sort(app_rank_pair_vector.begin(), app_rank_pair_vector.end(), 
            [](const d_pair& l, const d_pair& r) {
                    if (l.second != r.second)
                        return l.second > r.second;
                    return l.first.name > r.first.name;
            });
    
    // remove dubbles and empty entries
    for (int i = 0;i<app_rank_pair_vector.size();i++)
    {
        if (app_rank_pair_vector[i].first.name == QString(""))
            continue;
        status = true;
        for (int j = i+1;j<app_rank_pair_vector.size();j++)
        {
            if (app_rank_pair_vector[i].first.name == app_rank_pair_vector[j].first.name)
            {
                status = false;
                break;
            }
        }
        if (status)
        {
            applications.push_back(app_rank_pair_vector[i].first);
        }
    }

    for (int i = 0;i<buttons.size();i++)
    {
        if (!(i<applications.size()))
        {
            buttons[i]->hide();
            buttons[i]->setText(QString(""));
            continue;
        }
        buttons[i]->setText(applications[i].name);
        buttons[i]->setIcon(QIcon::fromTheme(applications[i].icon));
        buttons[i]->show();
    }
}

void MainLayout::exec()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    QString name;
    if (button != NULL)
        name = button->text();
    else
    {
        system((le->text()+QString(" &")).toStdString().c_str());
        QApplication::quit();
    }

    for (auto app : applications)
    {
        if (app.name == name)
        {
            std::cout << app.name.toStdString() << std::endl;
            std::cout << app.exec.toStdString() << std::endl;
            system((app.exec+QString(" &")).toStdString().c_str());
            QApplication::quit();
        }
    }
}

void MainLayout::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape)
    {
        QApplication::quit();
    }
}
