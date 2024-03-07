/* Фильтр Калмана feat. Alex Gyver */

void GKalman_setParameters(GKalman_HandlerTypeDef* local, float mea_e, float est_e, float _q);
float GKalman_Filtered(GKalman_HandlerTypeDef* local, float value);

typedef struct {
    float err_measure;
    float err_estimate;
    float q;
    float last_estimate;
} GKalman_HandlerTypeDef;

// разброс измерения, разброс оценки, скорость изменения значений
/* !
 * @brief GKalman_setParameters - установка параметров и инициализация фильтра Калмана
 * @param mea_e - разброс измерения
 * @param est_e - разброс оценки
 * @param q - скорость изменения значений
 *
 */
void GKalman_setParameters(GKalman_HandlerTypeDef* local, float mea_e, float est_e, float _q)
{
	local->err_measure = mea_e;
	local->err_estimate = est_e;
	local->q = _q;
	local->last_estimate = 0;
}

/* !
 * @brief GKalman_Filtered - фильтр Калмана
 * @param value - входное изменяющееся значение
 * @return value - отфильтрованное значение
 * 
 */
float GKalman_Filtered(GKalman_HandlerTypeDef* local, float value)
{		
    float kalman_gain, current_estimate;
	kalman_gain = local->err_estimate / (local->err_estimate + local->err_measure);
	current_estimate = local->last_estimate + kalman_gain * (value - local->last_estimate);
	local->err_estimate =  (1.0 - kalman_gain)*local->err_estimate + fabs(local->last_estimate-current_estimate)*local->q;
	local->last_estimate=current_estimate;
	return current_estimate;
}
