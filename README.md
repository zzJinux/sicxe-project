* 질문사항
  asemmble 커맨드에 확장자가 .asm 이 아닌 파일이 인자로 주어지면 어떻게 처리하나?
  (현재 구현: 에러 출력)

  .asm 포맷, label 길이, directive, opcode 길이, operand 길이, comment 길이
  label과 opcode를 구분할 수 있는 문법이 존재? or identifier로 판별해야 하나? -> 알아서 정할것
  
  format4로 표시된 instruction은 무조건 absolute address로 생성?
  현재: 무조건 32bit 차지하게 함

* Note
  malloc 하면 끝까지 추적하여 free 로 조지자.


* TODO
  NOBASE operand 존재유무
  identifier validator