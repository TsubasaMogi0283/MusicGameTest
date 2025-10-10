#pragma once
//音階表

//自分のエンジンではA4は442Hz基準にする
//もちろん12段階で1オクターブ
static const int SCALE_AMOUNT_ = 13;
const float SEMITONE_RATIO_[SCALE_AMOUNT_] = {
	1.00000f, //C4
	1.05946f, //C#4
	1.12246f, //D4
	1.18921f, //D#4
	1.25992f, //E4
	1.33483f, //F4
	1.41421f, //F#4
	1.49831f, //G4
	1.58740f, //G#4
	1.68179f, //A4
	1.78180f, //A#4
	1.88775f, //B4
	2.00000f  //C5
};

//低い方
const float MINUS_SEMITONE_RATION[SCALE_AMOUNT_] = {
	1.00000f,	//C4
	0.94387f,	//B3
	0.89090f,	//A3#
	0.84090f,	//A3
	0.79370f,	//G#3
	0.74915f,	//G3
	0.70711f,	//F#3
	0.66742f,	//F3
	0.62996f,	//E3
	0.59460f,	//D#3
	0.56123f,	//D3
	0.52973f,	//C#3
	0.50000f,	//C3
};