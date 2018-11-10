//drag만 지원하게 설정하였습니다. select의 size를 바꿔주지 않으면 오작동이 발생하여서 셀렉트 드래그 하는 동안 테이블의 높이가 바뀝니다.
function select_down(select) {  //셀렉트에서 마우스를 눌렀을때 저장할 select의 정보를 tmp_select_down에 저장하고 select를 펼칩니다.
    tmp_select_down = select;
    select.size = select.length;
}
function select_up(event) {
    if (tmp_select_down != null) {
        var curtop = 0;
        var obj = tmp_select_down;
        if (obj.offsetParent) { //tmp_select_down의 페이지 절대좌표 구하기.
            do {
                curtop += obj.offsetTop;
            } while (obj = obj.offsetParent);   //부모 요소가 있을 경우 반복하면서 최상위까지의 높이 합을 curtop에 저장합니다.
        }
        var height_option = document.getElementById('o' + tmp_select_down.getAttribute('id')).offsetHeight; //옵션의 높이 구하기.
        var select_pageY;
        if (tmp_select_down.getAttribute('id').substring(0, 2) == 'in') {
            select_pageY = event.clientY - curtop;    //select를 기준으로 이동거리를 저장.
        }
        else {
            select_pageY = event.pageY - curtop;
        }
        var next_idx = Math.floor(select_pageY / height_option);    //select가 선택할 value의 값을 구한다.
        if (tmp_select_down.getAttribute('id').substring(0, 2) == 'in') {   //모달의 경우 벨류값이 없는 옵션이 1개 있기 때문에 -1을 해준다.
            next_idx--;
        }
        if (next_idx < tmp_select_down.length) {    //next_idx가 0보다 작으면 자동적으로 0이 선택. next_idx가 범위 안이면 select의 value값을 next_idx로 고쳐준다.
            tmp_select_down.value = next_idx;
        } else {    //next_idx가 범위보다 크다면 가장 밑의 값으로 설정한다.
            if (tmp_select_down.getAttribute('id').substring(0, 2) == 'in') {
                tmp_select_down.value = tmp_select_down.length - 2;
            } else {
                tmp_select_down.value = tmp_select_down.length - 1;
            }
        }
        tmp_select_down.size = 0;   //셀렉트를 접습니다.
        if (tmp_select_down.getAttribute('id').substring(0, 2) != 'in') {   //모달의 경우 select_change를 호출하면 안됩니다.
            select_change(tmp_select_down); //드래그로 value값을 바꾸면 onchange 콜백함수가 작동하지 않습니다.
        }
    }
    tmp_select_down = null; //tmp_select_down를 null로 초기화.
}