#ifndef TOOLTIPWIDGET_H
#define TOOLTIPWIDGET_H

#include <QQuickWidget>

#include <QQmlEngine>

class ToolTipWidget : public QQuickWidget
{
public:
    ToolTipWidget(QQmlEngine *engine, QWidget *parent = nullptr);

    void setTargetRect(const QRect &rect);

protected:
    virtual void mouseMoveEvent(QMouseEvent *) override;

    virtual bool event(QEvent *) override;

private:
    QRect m_targetRect;
};

#endif // TOOLTIPWIDGET_H
