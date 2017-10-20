# AR-study   
# Interaction with OpenCV
•1. Multi-marker tracking   
o 2개의 마커를 추적   
o Homography로부터 카메라 좌표계에서마커좌표계로의 변환 행렬 추정   
o 마커좌표계 간의 거리 출력후 실제 거리와의 비교   
   
•2. Interaction   
o 지난 시간 수행한 묵, 찌, 빠를 이용하여 상호작용 수행     
  •추적한 다중 마커 위에 3차원 가상 객체를 증강(cube, teapot)      
o 손의 모양이 ‘묵’일 경우 x-y평면 이동 변환 및 z축 회전 변환   
  •이 때 두 마커 위의 3차원 객체는 톱니바퀴 회전      
o 1개가 시계 방향이면 다른 하나는 반 시계 방향으로 회전   
o 손의 모양이 ‘찌’일 경우 색상 변경   
  •R, G, B의 색상으로 변경(R->G->B)      
o 손의 모양이 ‘빠’일 경우   
  •Solid->wire로 변경     
   
# Image Processing with OpenCV
1. horizontal and vertical flip
2. rotation
3. scaling
4. hand pose estimation   
   
# Natural Marker based AR   
자연마커(Natural Feature Marker) 추적을 이용한 AR 기능 구현  
