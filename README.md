# NBC-UE5-3DigitBullsAndCows

블루프린트 함수 라이브러리 만들고, 숫자 생성, 정답 비교 같은 함수를 구현

PlayerController랑 GameMode 만들어서 RPC를 통해 둘 사이에 채팅 메시지를 주고 받을 수 있도록 구현

게임 진행을 위해 Blueprint Function Library에서 만든 함수들을 사용해서 숫자를 생성하고 이를 채팅으로 예측하고 판정 결과를 알려주도록 하는 게임의 기본 로직을 구현

PlayerState를 만들어서 각 플레이어의 시도 횟수를 가지도록 하여 예측할 때 마다 횟수를 차감하도록 하였고

턴과 제한 시간을 추가하기 위해 GameState를 만들어서 턴과 제한 시간을 관리
