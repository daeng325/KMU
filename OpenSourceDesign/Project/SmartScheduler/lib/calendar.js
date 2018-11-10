function getEventStart(data) {
    var result_date = new Date(parseInt(data.date.substring(0, 4)), parseInt(data.date.substring(4, 6) - 1), parseInt(data.date.substring(6, 8)));
    result_date = new Date(Date.parse(result_date) + 9 * 1000 * 60 * 60);
    if (data.state == 3) {
        result_date = new Date(Date.parse(result_date) + 1 * 1000 * 60 * 60 * 24);
    }
    return result_date.toJSON().substring(0, 10);
}

function addEvents(arr) {
    var year, month, day;

    for (var i = 0; i < arr.length; i++) {
        for (var j = 0; j < arr[i].save.length; j++) {
            if (arr[i].save[j].state != 1 && arr[i].save[j].state != 4) {
                //year = arr[i].save[j].date.substring(0, 4); //객체에 날짜가 '20161108'으로 들어간 것을 '2016-11-08'로 고치는 작업입니다.
                //month = arr[i].save[j].date.substring(4, 6);
                //day = arr[i].save[j].date.substring(6, 8);

                var MyCalendar = $('#f_calendar');

                var myEvent = { //이벤트 객체 생성.
                    title: arr[i].save[j].schedule,
                    start: getEventStart(arr[i].save[j]),
                    upper: arr[i].save[j].upper,
                    lower: arr[i].save[j].lower
                };

                MyCalendar.fullCalendar('renderEvent', myEvent, true);  //이벤트를 캘린더에 넣는 명령어 입니다. 3번째 인자값은 계속 존재하는지에 대한 여부를 물어봅니다.
            }
        }
    }
} //처음 Calendar를 초기화 할 때 DB에서 값을 받아와서 초기화

function addEvent(data) {
    year = data.date.substring(0, 4);
    month = data.date.substring(4, 6);
    day = data.date.substring(6, 8);

    var MyCalendar = $('#f_calendar');

    var myEvent = {
        title: data.schedule,
        start: getEventStart(data),
        upper: data.upper,
        lower: data.lower
    };
    MyCalendar.fullCalendar('renderEvent', myEvent, true);
} //테이블에서 추가할 때 1개씩 Calendar에 추가하는 부분입니다.

function render_cal() { //calendar를 초기화합니다.
    $('#f_calendar').fullCalendar('removeEvents');  //모든 정보를 삭제합니다.
    addEvents(arr_sort);    //arr_sort의 정보를 event에 입력합니다.
}