/**
 * @file MBNet_1000.cpp
 * @brief Detect object type
 * @author Neucrack@sipeed.com
 */


#include "MBNet_1000.h"
#include "names.h"
#include "stdlib.h"
#include "errno.h"


MBNet_1000::MBNet_1000(KPUClass& kpu, Sipeed_ST7789& lcd, Sipeed_OV2640& camera)
:_kpu(kpu),_lcd(lcd), _camera(camera),
_model(nullptr), _count(0), _result(nullptr)
{
    _names = mbnet_label_name;
    memset(_statistics, 0, sizeof(_statistics));
}

MBNet_1000::~MBNet_1000()
{
    if(_model)
        free(_model);
}


int MBNet_1000::begin(const char* kmodel_name)
{
    File myFile;
    if(!_camera.begin())
        return -1;
    if(!_lcd.begin(15000000, COLOR_RED))
        return -2;
    _camera.run(true);
    _lcd.setTextSize(2);
    _lcd.setTextColor(COLOR_WHITE);

    if (!SD.begin()) 
    {
        _lcd.setCursor(100,100);
        _lcd.print("No SD Card");
        return -3;
    }

    myFile = SD.open(kmodel_name);
    if (!myFile)
        return -4;
    uint32_t fSize = myFile.size();
    _lcd.setCursor(100,100);
    _lcd.print("Loading ... ");
    _model = (uint8_t*)malloc(fSize);
    if(!_model)
    {
        _lcd.setCursor(100,140);
        _lcd.print("Memmory not enough ... ");
        return ENOMEM;
    }
    long ret = myFile.read(_model, fSize);
    myFile.close();
    if(ret != fSize)
    {
        free(_model);
        _model = nullptr;
        return -5;
    }

    if(_kpu.begin(_model) != 0)
    {
        free(_model);
        _model = nullptr;
        return -6;
    }
    return 0;
}

int MBNet_1000::detect()
{
    uint8_t* img;
    uint8_t* img888;

    img = _camera.snapshot();
    if(img == nullptr || img==0)
        return -1;
    img888 = _camera.getRGB888();
    if(_kpu.forward(img888) != 0)
    {
        return -2;
    }
    while( !_kpu.isForwardOk() );
    if( _kpu.getResult((uint8_t**)&_result, &_count) != 0)
    {
        return -3;
    }
    return 0;
}

void MBNet_1000::show()
{
    float prob;
    const char* name;
    uint8_t i, j;
    uint16_t* img;

    _count /= sizeof(float);
    label_init();
    label_sort();

    for(j=0; j<STATISTICS_NUM; ++j)
        _statistics[j].updated = false;
    for ( i = 0; i < 5; i++)
	{
		label_get(i, &prob, &name);
        for(j=0; j<STATISTICS_NUM; ++j)
        {
            if(_statistics[j].name == NULL)
            {
                _statistics[j].name = name;
                _statistics[j].sum  = prob;
                _statistics[j].updated = true;
                break;
            }
            else if( _statistics[j].name == name )
            {
                _statistics[j].sum += prob;
                _statistics[j].updated = true;
                break;
            }
            else
            {
            }
        }
        if( j == STATISTICS_NUM)
        {
            float min = _statistics[0].sum;
            j = 0;
            for(i=1; i<STATISTICS_NUM; ++i)
            {
                if(_statistics[i].name)
                {
                    if(_statistics[i].sum <= min)
                    {
                        min = _statistics[i].sum;
                        j = i;
                    }
                }
            }
            _statistics[j].name = name;
            _statistics[j].sum  = prob;
            _statistics[j].updated = true;
        }
    }
    float max = _statistics[0].sum;
    float second = 0;
    uint8_t index1=0, index2 = 0;
    for(i=0; i<STATISTICS_NUM; ++i)
    {
        if(_statistics[i].name)
        {
            if(_statistics[i].sum > max)
            {
                max = _statistics[i].sum;
                index1 = i;
            }
            else if(_statistics[i].sum > second && _statistics[i].sum<max)
            {
                index2 = i;
            }
        }
        if( !_statistics[i].updated )
        {
            float tmp = _statistics[i].sum - _statistics[i].sum*2/5;
            if( tmp < 0)
                tmp = 0;
            _statistics[i].sum = tmp;
        }
    }
    img = _camera.getRGB565();
    _lcd.fillRect(224,0, _lcd.width()-224, _lcd.height(), COLOR_RED);
    _lcd.drawImage(0, 0, _camera.width(), _camera.height(), img);
    _lcd.setTextSize(2);
    _lcd.setTextColor(COLOR_WHITE);
    _lcd.setCursor(0,0);
    _lcd.println(_statistics[index1].name);
    _lcd.println("-----");
    _lcd.println(_statistics[index2].name);
    // _lcd.println("=======");
    // _lcd.println(_names[_index[0]]);
    // _lcd.println(_names[_index[1]]);
    // _lcd.println(_names[_index[2]]);
    // _lcd.println(_names[_index[3]]);
    // _lcd.println(_names[_index[4]]);
}


void MBNet_1000::label_init( )
{
	int i;
	for(i = 0; i < _count; i++)
		_index[i] = i;
}

void MBNet_1000::label_sort(void)
{
    int i,j;
    float tmp_prob;
    uint16_t tmp_index;
    for(j=0; j<_count; j++)
        for(i=0; i<_count-1-j; i++)
            if(_result[i]<_result[i+1])
            {
                tmp_prob=_result[i];
                _result[i]=_result[i+1];
                _result[i+1]=tmp_prob;
                
                tmp_index=_index[i];
                _index[i]=_index[i+1];
                _index[i+1]=tmp_index;
            }
}


void MBNet_1000::label_get(uint16_t index, float* prob, const char** name)
{
	*prob = _result[index];
	*name = _names[_index[index]];
}

