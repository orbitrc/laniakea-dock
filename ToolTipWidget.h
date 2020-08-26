#ifndef TOOLTIPWIDGET_H
#define TOOLTIPWIDGET_H

#include <QQuickWidget>

#include <QQmlEngine>

class ToolTipWidget : public QQuickWidget
{
    Q_OBJECT
public:
    ToolTipWidget(QQmlEngine *engine, QWidget *parent = nullptr);
    ~ToolTipWidget();

    void setTargetRect(const QRect &rect);

protected:
    virtual void closeEvent(QCloseEvent *) override;
    virtual void mouseMoveEvent(QMouseEvent *) override;

    virtual bool event(QEvent *) override;

signals:
    void closed();

private:
    QRect m_targetRect;
};

#endif // TOOLTIPWIDGET_H
