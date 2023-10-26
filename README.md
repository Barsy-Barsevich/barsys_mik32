# barsys_mik32

v1.4 (26.10.2023)
- добавлена функция HAL_TSENS_MspInit()
- поправлена функция HAL_TSENS_Init()
- в операторах switch-case поставлены константы препроцессора
- в функции HAL_TSENS_ReadMeasurement() макрос вычисления температуры заменен на формулы
- мелкие правки

TODO:
- оптимизировать формулы преобразования сырых значений в температуру; вывод в градусах
- учесть особенности делителя MIK32V2
- привести порядок в файле-примере, отладить баги
