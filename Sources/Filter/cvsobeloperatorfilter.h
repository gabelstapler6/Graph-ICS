#ifndef CVSOBELOPERATORFILTER_H
#define CVSOBELOPERATORFILTER_H

#include "node.h"

class CvSobelOperatorFilter : public Node
{
    Q_OBJECT
    Q_PROPERTY(int xDerivative READ getXDerivative WRITE setXDerivative NOTIFY xDerivativeSizeChanged)
    Q_PROPERTY(int yDerivative READ getYDerivative WRITE setYDerivative NOTIFY yDerivativeSizeChanged)

public:
    explicit CvSobelOperatorFilter();
    virtual ~CvSobelOperatorFilter() {}

    virtual bool retrieveResult();

    int getXDerivative() const;
    void setXDerivative(int value);

    int getYDerivative() const;
    void setYDerivative(int value);

signals:
    void xDerivativeSizeChanged();
    void yDerivativeSizeChanged();

private:
    int m_xDerivative;
    int m_yDerivative;
    int m_ddepth;     //Depth of the image
};

#endif // CVSOBELOPERATORFILTER_H
