/** history.h
 * history 관리 모듈 헤더
 */
#ifndef _HISTORY_H_
#define _HISTORY_H_

#include "shell-context.h"

/** initHistory
 *  커맨드 history 리스트 초기화 함수
 * 
 *  @인수
 *    pContext - 프로그램 상태
 */
void initHistory(ShellContextPtr pContext);

/** updateHistory
 *  text를 history에 추가함
 * 
 *  @인수
 *    pContext - 프로그램 상태
 *    text - history에 추가할 텍스트(커맨드 입력)
 *    len - text의 길이
 * 
 *  @반환
 *    새로 추가한 history 노드의 포인터
 */
HistNodePtr updateHistory(ShellContextPtr pContext, char const* text, int len);

/** cleanupHistory
 *  history 구조에 사용된 자원 해제
 * 
 *  @인수
 *    pContext - 프로그램 상태
 */
void cleanupHistory(ShellContextPtr pContext);

/** printHistory
 *  history 목록 출력, 최근일 수록 목록의 밑에 있음
 * 
 *  @인수
 *    pContext - 프로그램 상태
 */
void printHistory(ShellContextPtr pContext);

#endif