# EQIN_Alarm

-------------------------------------------------------------------

**GY-61 ADXL335(아날로그 가속도 센서) 및 ATmega128A를 사용한 지진 및 층간소음 알리미 개발

-------------------------------------------------------------------

## Developer & Development_Part

### 손영동(PM)
- GY-61 ADXL335(아날로그 가속도 센서)x, y, z 센서 값 데이터 보정
- 지진 및 층간소음 별 부저 사운드 제어
- 최종 소스코드 병합 및 수정

### 장지원
- 하우스 프로토타입 프레임 개발
- 상황별 3색 LED센서 동작 제어(평상시 : 녹색 | 지진 : 빨간색 | 층간소음 : 파랑색| 센서 오류 : 보라색 ) 

### 박하용
- GY-61 ADXL335(아날로그 가속도 센서)x, y, z 센서 값 데이터 보정

### 박찬영
- LCD 함수 
- 지진 및 층간소음 별 부저 사운드 제어

-------------------------------------------------------------------

## Operation Structure
### 사용변수
#### 전역변수
- int in_persent 
// 층간소음 정도를 백분율로 계산한 결과

- int judgment_flag 
// 평상시(0), 지진(1), 층간소음(2), 센서 에러(3) 판단 플래그

- int eq_max 
// 3번이상 검출시 int judgment_flag = 1

- int in_max 
// 3번이상 검출시 int judgment_flag = 2

- int in_save
//층간소음이 발생할 당시의 in_persent를 저장

- int in_save_flag
//in_save를 한번만 하기 위한 플래그

- int eq_i;
//Delay를 덜 쓰면서 shift하기 위한 변수
#### 지역변수
- 함수별 지역변수는 개발하면서 추가바람!!

- control_value_analysis()  
	->  
  
- lcd_control()  
	->   char buf[100];
  
- led_control()  
	->  	
  
- buzzer_control()  
	-> 
  
### 사용함수
#### control_value_analysis()[손영동, 박하용]
- GY-61 ADXL335(아날로그 가속도 센서)를 이용하여 지진 및 층간소음 분석 및 검출
- 분석이 완료되면 그에 따른 값을 변수에 저장
- 가속도 센서 안정권(eq_max, in_max가 값을 넘치 않을 경우)이 3번이상 발생시 int judgment_flag = 0
- 지진 발생 int eq_max값을 3번이상 넘어가면 int judgment_flag = 1 
- 층간 소음 int in_max값을 3번이상 넘어가면  int judgment_flag = 2
- int eq_max 0으로 초기화 = int eq_max값이 3번이상 넘어감, in_max값이 3번이상 넘어감, 가속도 센서 안정권이면 0으로 초기화
- int in_max 0으로 초기화 = int in_max값이 3번이상 넘어감, eq_max값이 3번이상 넘어감, 가속도 센서 안정권이면 0으로 초기화
- int z_adc 값이 300미만이 되었을 때 int judgment_flag = 3 

#### lcd_control()[박찬영]
- 평상시, 지진, 층간소음에 따른 lcd 정보 출력
- int judgment_flag = 0이면 Normal이라는 스트링을 표시하고 층간소음 퍼센트 정도를 int in_persent에서 받아와 표시
- 		    = 1이면 Earthquake이라는 스트링을 표시하고 층간소음 퍼센트 정도를 int in_persent에서 받아와 표시
- 		    = 1이면 두번째 줄에 지진 대피 요령 출력(0.3초씩 글자 한칸씩 왼쪽으로 쉬프트)  
- 		    = 2이면 Interlayer noise이라는 스트링을 표시하고 순간 층간소음 퍼센트 정도를 int in_persent에서 받아와 int save_persent에 저장후 int save_persent 정보 출력
-		    = 3이면 ERROR표시 출력

#### led_control()[장지원]
- 평상시, 지진, 층간소음에 따른 led 출력
- int judgment_flag 값 변동에 따라 한번만 호출되는 함수
- int judgment_flag = 0이면 평상시로 초록색 led 출력
- 		    = 1이면 지진으로 빨간색 led 출력
- 		    = 2이면 층간소음으로 파랑색 led 출력
-		    = 3이면 센서 에러로 보라색 led 출력  

#### buzzer_control()[손영동, 박찬영]
- 평상시, 지진, 층간소음에 따른 buzzer 소리 출력 
- int judgment_flag = 0이면 평상시로 buzzer 소리 출력 안함
- 		    = 1이면 지진으로 buzzer로 int in_persent값에 따라 pwd으로 소리크기를 조절하여 출력
- 		    = 2이면 층간소음으로 buzzer로 0.5초간 큰 소리 출력 및 꺼짐 6번 반복(총 3초) 	

-------------------------------------------------------------------

## Pin placement
- control_value_analysis()  
	->  x축 = PF0  
	->  y축 = PF1  
	->  z축 = PF2  
  
- lcd_control()  
	->  PE0~7  
  
- led_control()  
	->  R(Red) = PC0  
	->  G(Green) = PC1  
	->  B(Blue) = PC2  
  
- buzzer_control()  
	->  PB5(OCRA)  

- interrupt button()  
	->  PD0  

-------------------------------------------------------------------

## Main logic(Algorithm)
```c
#include <stdio.h>
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

//전역변수 선언
int in_persent = 0; // 층간소음 정도를 백분율로 계산한 결과
int judgment_flag = 0; // 평상시(0), 지진(1), 층간소음(2) 판단 플래그
int eq_max = 0; // 3번이상 검출시 int judgment_flag = 1
int in_max = 0; // 3번이상 검출시 int judgment_flag = 2

int control_value_analysis(void);
int lcd_control(void);
int led_control(void);	
int buzzer_control(void);

int main(void)
{
	while(1)
	{
		control_value_analysis();
		led_control();
		buzzer_control();
		lcd_control();
	}
	return 0;
}
```
- 추가적인 로직은 팀간의 상의를 통해 구성 예정
