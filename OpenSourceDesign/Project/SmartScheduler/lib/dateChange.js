function move_yesterday(event) //전날로 이동
{
    var yesterday = new Date(Date.parse(current_date) - 1 * 1000 * 60 * 60 * 24);
    current_date = yesterday;
    init();

}
function move_tomorrow(event) //다음날로 이동
{
    var tomorrow = new Date(Date.parse(current_date) + 1 * 1000 * 60 * 60 * 24);
    current_date = tomorrow;
    init();
}
function init() { //날짜 출력 실행
    var date_Json_tmp = new Date(Date.parse(current_date) + 9 * 1000 * 60 * 60);
    document.getElementById('current_date').innerHTML = date_Json_tmp.toJSON().substring(0, 10);
    document.getElementById('in_date').value = date_Json_tmp.toJSON().substring(0, 10);    //이 부분이 JSON방식으로 입력을 받기 때문에 $.ready로 1번 9시간을 더해서 보정하였습니다. 추가 모달의 기본 날짜 설정.
    document.getElementById('date_moving').value = date_Json_tmp.toJSON().substring(0, 10);
    table_change(current_date.getFullYear(), current_date.getMonth() + 1, current_date.getDate());
}
function move_today(event) {    //오늘 날짜로 이동.
    var today = new Date();
    current_date = today;
    init();
}
function move_date(event) //옆에 박스에서 날짜를 바꿔주고 이동을 누르면 그 날짜로 이동됨
{
    var tmp_date = document.getElementById('date_moving').value;
    tmp_date = tmp_date.split('-');
    current_date.setFullYear(parseInt(tmp_date[0]), parseInt(tmp_date[1]) - 1, parseInt(tmp_date[2]));
    init();
}
function table_change(year, month, date) //날짜가 바뀌면 그 날짜의 할일목록에 가서 할일목록을 출력함.
{
    var compareDate = year * 10000 + month * 100 + date; //arr_non_sort에 date가 들어있는 방식과 같게하여 원활한 비교를 위해 YYYYMMDD로 바꿔줍니다.
    var text = '<table id="text_table" class="table table-bordered"><tr><th class="text-center">상태<th class="text-center">상위<th class="text-center">하위<th class="text-center">할일<th class="text-center" style="border-top: 1px solid #ffffff; border-right: 1px solid #ffffff;  border-bottom: 1px solid #ffffff;"></tr>'; //테이블의 첫번째줄
    var correct_idx = -1; //배열 위치 확인용. 없으면 -1값을 가지게 됩니다.
    if (!sort_flag) {
        for (var i = 0 ; i < arr_non_sort.length ; i++) {
            if (compareDate == parseInt(arr_non_sort[i].date)) {
                correct_idx = i;
                for (var j = 0; j < arr_non_sort[correct_idx].save.length; j++) {
                    text = text.concat('<tr>\
                            <td><select id="s0_' + arr_non_sort[i].save[j].id + '" onchange="select_change(this)" onmousedown="select_down(this)"><option value="0">*</option><option value="1">V</option><option value="2" id="os0_' + arr_non_sort[i].save[j].id + '">-</option><option value="3">- ></option><option value="4">X</option></select></td>\
                            <td><select id="s1_' + arr_non_sort[i].save[j].id + '" onchange="select_change(this)" onmousedown="select_down(this)"><option value="0">A</option><option value="1">B</option><option value="2" id="os1_' + arr_non_sort[i].save[j].id + '">C</option><option  value="3">D</option></select></td>\
                            <td><select id="s2_' + arr_non_sort[i].save[j].id + '" onchange="select_change(this)" onmousedown="select_down(this)"><option value="0">1</option><option value="1">2</option><option value="2" id="os2_' + arr_non_sort[i].save[j].id + '">3</option><option  value="3">4</option></select></td>\
                            <td><input id="i0_' + arr_non_sort[i].save[j].id + '" onblur="select_change(this)" type = "text" value="' + arr_non_sort[i].save[j].schedule + '" size="40" style="border: 0px none;"></input></td>\
                            <td style="border-top: 1px solid #ffffff; border-right: 1px solid #ffffff; border-bottom: 1px solid #ffffff;"><button class="btn_style_delete" id="b0_' + arr_non_sort[i].save[j].id + '" onclick=delete_tr(this)><span class="glyphicon glyphicon-remove"></span></button></td>\
                            </tr>');
                }
            }
        }
        text = text.concat("</table>");
        document.getElementById('text_table_div').innerHTML = text;
        if (correct_idx != -1) { //할일이 없는경우 윗부분만 출력되게 함.
            for (var j = 0; j < arr_non_sort[correct_idx].save.length; j++) {
                document.getElementById("s0_" + arr_non_sort[correct_idx].save[j].id).value = arr_non_sort[correct_idx].save[j].state;
                document.getElementById("s1_" + arr_non_sort[correct_idx].save[j].id).value = arr_non_sort[correct_idx].save[j].upper;
                document.getElementById("s2_" + arr_non_sort[correct_idx].save[j].id).value = arr_non_sort[correct_idx].save[j].lower;
            }
        }
    } else {
        for (var i = 0 ; i < arr_sort.length ; i++) {
            if (compareDate == parseInt(arr_sort[i].date)) {
                correct_idx = i;
                for (var j = 0; j < arr_sort[correct_idx].save.length; j++) {
                    text = text.concat('<tr>\
                            <td><select id="s0_' + arr_sort[i].save[j].id + '" onchange="select_change(this)" onmousedown="select_down(this)"><option value="0">*</option><option value="1">V</option><option value="2" id="os0_' + arr_sort[i].save[j].id + '">-</option><option value="3">- ></option><option value="4">X</option></select></td>\
                            <td><select id="s1_' + arr_sort[i].save[j].id + '" onchange="select_change(this)" onmousedown="select_down(this)"><option value="0">A</option><option value="1">B</option><option value="2" id="os1_' + arr_sort[i].save[j].id + '">C</option><option  value="3">D</option></select></td>\
                            <td><select id="s2_' + arr_sort[i].save[j].id + '" onchange="select_change(this)" onmousedown="select_down(this)"><option value="0">1</option><option value="1">2</option><option value="2" id="os2_' + arr_sort[i].save[j].id + '">3</option><option  value="3">4</option></select></td>\
                            <td><input id="i0_' + arr_sort[i].save[j].id + '" onblur="select_change(this)" type = "text" value="' + arr_sort[i].save[j].schedule + '" size="40" style="border: 0px none;"></input></td>\
                            <td style="border-top: 1px solid #ffffff; border-right: 1px solid #ffffff; border-bottom: 1px solid #ffffff;"><button class="btn_style_delete" id="b0_' + arr_sort[i].save[j].id + '" onclick=delete_tr(this)><span class="glyphicon glyphicon-remove"></span></button></td>\
                            </tr>');
                }
            }
        }
        text = text.concat("</table>");
        document.getElementById('text_table_div').innerHTML = text;
        if (correct_idx != -1) { //할일이 없는경우 윗부분만 출력되게 함.
            for (var j = 0; j < arr_sort[correct_idx].save.length; j++) {
                document.getElementById("s0_" + arr_sort[correct_idx].save[j].id).value = arr_sort[correct_idx].save[j].state;
                document.getElementById("s1_" + arr_sort[correct_idx].save[j].id).value = arr_sort[correct_idx].save[j].upper;
                document.getElementById("s2_" + arr_sort[correct_idx].save[j].id).value = arr_sort[correct_idx].save[j].lower;
            }
        }
    }
}