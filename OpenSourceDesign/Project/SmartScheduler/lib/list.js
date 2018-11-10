function togle_hide() { //달력형에서는 현재 날짜와 이동버튼이 따로 필요없다고 생각해서 지우는 기능을 추가하였습니다.
    flag_hide = !flag_hide;
    var date_div = document.getElementById('date_info');
    if (flag_hide) {
        date_div.style.display = "none";
    } else {
        date_div.style.display = "block";
    }
}

function delete_tr(test) {  //삭제는 목록형에서만 지원하기 때문에 tr을 삭제하면서 db를 삭제합니다.
    table = document.getElementById('text_table');
    delete_db(test.getAttribute('id')); //id값은 고유하기 때문에 id값을 이용해서 db의 정보를 삭제합니다.
    for (var i = 1 ; i < $('table tr').length; i++) {
        if ($('#text_table button').eq(i - 1).attr('id').split('_')[1] == test.getAttribute('id').split('_')[1]) {    //id의 값은 고유한 번호이기 때문에 만약 select의 state가 X일때 삭제해야하면 select에 의해 호출이 되어도 button으로 비교해도 무방합니다.
            table.deleteRow(i); //테이블에서의 삭제.
            break;
        }
    }
    var compareDate = current_date.getFullYear() * 10000 + (current_date.getMonth() + 1) * 100 + current_date.getDate();
    for (var i = 0 ; i < arr_non_sort.length ; i++) {   //배열에서의 삭제.
        if (compareDate == parseInt(arr_non_sort[i].date)) {
            for (var j = 0; j < arr_non_sort[i].save.length; j++) {
                if (arr_non_sort[i].save[j].id == test.getAttribute('id').split('_')[1]) {
                    arr_non_sort[i].save.splice(j, 1);  //j를 기준으로 1개 삭제.
                }
            }
        }
        if (compareDate == parseInt(arr_sort[i].date)) {
            for (var j = 0; j < arr_sort[i].save.length; j++) {
                if (arr_sort[i].save[j].id == test.getAttribute('id').split('_')[1]) {
                    arr_sort[i].save.splice(j, 1);
                }
            }
        }
    }
    render_cal();
}

function add_db() {
    var in_state = document.getElementById('in_state').value;   //모달의 입력창에서 값을 받아옵니다.
    var in_upper = document.getElementById('in_upper').value;
    var in_lower = document.getElementById('in_lower').value;
    var in_schedule = document.getElementById('in_schedule').value;
    var date_text = document.getElementById('in_date').value.split('-');
    var in_date = date_text[0] + date_text[1] + date_text[2];
    var tmp_obj = {
        state: in_state,
        upper: in_upper,
        lower: in_lower,
        schedule: in_schedule,
        date: in_date,
        id: -1  //id값은 현재 모르기 때문에 -1값을 줍니다.
    }
    $.post("./lib/php/ajax_db_insert.php", tmp_obj,
    function (data) {   //echo로 받은 id값인 data를 포함해서 변수를 만든 다음 배열에 추가합니다.
        tmp_obj.id = data;
        var flag = true;   //배열 안에 존재하는지에 대한 flag변수. arr_non_sort와 arr_sort는 순서만 다르고 값은 같기 때문에 1개의 flag로 가능합니다.
        var len = arr_non_sort.length;  //push에 따른 길이 변동을 막기 위해 변수로 받아놓습니다.
        for (var i = 0; i < len; i++) { //날짜별로 분류하였기 때문에 선형탐색을 하더라도 매우 적은 시간이 듭니다.
            if (arr_non_sort[i].date == in_date) {
                arr_non_sort[i].save.push(tmp_obj);
                flag = false;
            }
            if (arr_sort[i].date == in_date) {
                arr_sort[i].save.push(tmp_obj);
                arr_sort[i].save.sort(function (left, right) {
                    if (parseInt(left.upper) == parseInt(right.upper)) {
                        return parseInt(left.lower) - parseInt(right.lower);
                    } else {
                        return parseInt(left.upper) - parseInt(right.upper);
                    }
                });
                flag = false;
            }
        }
        if (flag) { //날짜에 최초로 데이터가 들어왔을때. 새로운 객체를 만들어서 추가해줍니다.
            arr_sort.push({
                date: in_date,
                save: [tmp_obj]
            });
            arr_non_sort.push({
                date: in_date,
                save: [tmp_obj]
            });
        }
        addEvent(tmp_obj);  //calendar 추가부분. 캘린더 초기화는 탭이 변할때 콜백함수로 초기화 됩니다.
        table_change(current_date.getFullYear(), current_date.getMonth() + 1, current_date.getDate());  //테이블 초기화.
    })
}
function delete_db(in_id) { //id값을 받아와서 ajax의 post타입으로 ajax_db_delete.php와 통신을 하여서 db의 in_id에 해당하는 데이터를 삭제합니다.
    in_id = in_id.split('_');
    in_id = in_id[1];
    $.post('./lib/php/ajax_db_delete.php', { id: in_id });
}

function select_change(select) {    //select가 바뀌거나, schedule이 바뀔경우 실행. schedule은 포커스를 잃을 때 발생합니다.
    var select_info = select.getAttribute('id').split('_');
    var tmp = {
        state: document.getElementById('s0_' + select_info[1]).value,
        upper: document.getElementById('s1_' + select_info[1]).value,
        lower: document.getElementById('s2_' + select_info[1]).value,
        schedule: document.getElementById('i0_' + select_info[1]).value,
        date: current_date.getFullYear() * 10000 + (current_date.getMonth() + 1) * 100 + current_date.getDate() + '',
        id: select_info[1]
    }
    for (var i = 0; i < arr_non_sort.length; i++) { //배열에 업데이트
        if (arr_non_sort[i].date == tmp.date) {
            for (var j = 0; j < arr_non_sort[i].save.length; j++) {
                if (arr_non_sort[i].save[j].id == tmp.id) {
                    arr_non_sort[i].save[j].state = tmp.state;
                    arr_non_sort[i].save[j].upper = tmp.upper;
                    arr_non_sort[i].save[j].lower = tmp.lower;
                    arr_non_sort[i].save[j].schedule = tmp.schedule;
                    arr_non_sort[i].save[j].date = tmp.date;
                }
            }
        }
        if (arr_sort[i].date == tmp.date) {
            for (var j = 0; j < arr_sort[i].save.length; j++) {
                if (arr_sort[i].save[j].id == tmp.id) {
                    arr_sort[i].save[j].state = tmp.state;
                    arr_sort[i].save[j].upper = tmp.upper;
                    arr_sort[i].save[j].lower = tmp.lower;
                    arr_sort[i].save[j].schedule = tmp.schedule;
                    arr_sort[i].save[j].date = tmp.date;
                    arr_sort[i].save.sort(function (left, right) {
                        if (parseInt(left.upper) == parseInt(right.upper)) {
                            return parseInt(left.lower) - parseInt(right.lower);
                        } else {
                            return parseInt(left.upper) - parseInt(right.upper);
                        }
                    });
                }
            }
        }
    }
    table_change(current_date.getFullYear(), (current_date.getMonth() + 1), current_date.getDate());
    update_db(tmp); //db에 업데이트
    render_cal();   //calendar 초기화.
}
function update_db(update_data) {
    $.post('./lib/php/ajax_db_update.php', update_data);    //ajax_db_update.php를 통해서 db의 값을 업데이트 합니다.
}