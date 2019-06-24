// Copyright (c) 2019 hors<horsicq@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
#include "modevalidator.h"

ModeValidator::ModeValidator(QObject *parent) : QValidator(parent)
{
    data={};
}

void ModeValidator::setData(ModeValidator::DATA data)
{
    this->data=data;
}

QValidator::State ModeValidator::validate(QString &input, int &pos) const
{
    QValidator::State result=Acceptable;

    if(!input.isEmpty())
    {
        result=Invalid;

        bool bSuccess=false;
        quint64 nValue=0;

        if(data.mode==MODE_HEX)
        {
            nValue=input.toULongLong(&bSuccess,16);
        }
        else if(data.mode==MODE_SIGNED)
        {
            nValue=(quint64)input.toLongLong(&bSuccess,10);
        }
        else if(data.mode==MODE_UNSIGNED)
        {
            nValue=input.toULongLong(&bSuccess,10);
        }

        if(bSuccess&&(nValue<=data.nMaxValue))
        {
            result=Acceptable;
        }
    }

    return result;
}
