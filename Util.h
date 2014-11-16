
typedef cv::Rect_<double> RECTD;

template<typename _Ty>
_Ty& LimitMax(_Ty &Val, const _Ty &vMax)
{
	if (Val > vMax)
	{
		Val = vMax;
	}
	return Val;
}

template<typename _Ty>
_Ty& LimitMin(_Ty &Val, const _Ty &vMin)
{
	if (Val < vMin)
	{
		Val = vMin;
	}
	return Val;
}
