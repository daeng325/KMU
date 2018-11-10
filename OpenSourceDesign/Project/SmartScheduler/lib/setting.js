var arr_non_sort = [];  //날짜별로 1차원 객체 배열. 속성 save안에 테이블 한 줄에 대한 정보가 배열로 있다. save를 정렬 안하고 등록 순으로 받은 배열.
var arr_sort = [];  //arr_non_sort의 save를 우선순위에 따라서 정렬한 배열.
var current_date = new Date();  //목록형 화면의 날짜를 저장. 기본값은 현재날짜.
var sort_flag = false;  //false: 등록순위, true: 우선순위
var tmp_select_down;
var flag_hide = false;

$(document).ready(function () {
    $('#addModal').keydown(function (e) {
        if (e.keyCode == 13) {
            e.cancelBubble = true;
            $('#save').click();
            return false;
        }
    });

    $('a[data-toggle="tab"]').on('shown.bs.tab', function (e) { //부트스트랩의 a태그의 data-toggle속성이 tab인 것들의 shown.bs.tab콜백함수 재정의
        $('#f_calendar').fullCalendar('render');    //캘린더를 render한다.
        $('#f_calendar').fullCalendar('rerenderEvents');    //캘린더의 event들을 render한다. 추가한 것을 보여주도록 함.
        togle_hide();
    });

    $("#f_calendar").fullCalendar({
        defaultDate: current_date,
        editable: false,

        timeFormat: 'h:mm',
        columnFormat: 'ddd',
        titleFormat: 'YYYY MMMM',
        monthNames: ["1월", "2월", "3월", "4월", "5월", "6월", "7월", "8월", "9월", "10월", "11월", "12월"],
        monthNamesShort: ["1월", "2월", "3월", "4월", "5월", "6월", "7월", "8월", "9월", "10월", "11월", "12월"],
        dayNames: ["일요일", "월요일", "화요일", "수요일", "목요일", "금요일", "토요일"],
        dayNamesShort: ["일", "월", "화 ", "수 ", "목 ", "금 ", "토 "],
        buttonText: {
            today: "오늘"
        },
        eventLimit: true,
        eventOrder: ["upper", "lower"],  //calendar를 upper, lower로 정렬해서 보여줌.
        dayClick: function (date, jsEvent, view) {  //날짜를 눌렀을 때 리스트의 해당 날짜로 이동.
            current_date.setFullYear(date.format().substring(0, 4), (parseInt(date.format().substring(5, 7)) - 1), date.format().substring(8, 10));    //리스트의 현재날짜 수정.
            $('#myTab a:first').tab('show');    //리스트로의 이동
            init(); //날짜 텍스트와 테이블 초기화
            $('#myTab a:last').trigger('blur');
        },
        eventClick: function (calEvent, jsEvent, view) {    //이벤트를 눌렀을 때 리스트의 해당 날짜로 이동.
            $('#f_calendar').fullCalendar('option', 'dayClick')(calEvent._start, jsEvent, view);    //속성 dayClick을 호출한다.
        }
    });

    $("#date_moving").datepicker({
        changeMonth: true,
        changeYear: true,
        nextText: '다음 달',
        prevText: '이전 달',
        dateFormat: "yy-mm-dd",
        dayNamesMin: ['일', '월', '화', '수', '목', '금', '토'],
        monthNamesShort: ['1', '2', '3', '4', '5', '6', '7', '8', '9', '10', '11', '12']
    });
    $("#in_date").datepicker({
        changeMonth: true,
        changeYear: true,
        nextText: '다음 달',
        prevText: '이전 달',
        dateFormat: "yy-mm-dd",
        dayNamesMin: ['일', '월', '화', '수', '목', '금', '토'],
        monthNamesShort: ['1', '2', '3', '4', '5', '6', '7', '8', '9', '10', '11', '12']
    });

    var xmlhttp;
    try {
        xmlhttp = new XMLHttpRequest();
    } catch (e) {
        var version = [
            'Msxml2.XMLHTTP.6.0',
            'Msxml2.XMLHTTP.5.0',
            'Msxml2.XMLHTTP.4.0',
            'Msxml2.XMLHTTP.3.0',
            'Msxml2.XMLHTTP',
            'Microsoft.XMLHttp'
        ];
        for (var i = 0; i < version.length; i++) {
            try {
                xmlhttp = new ActiveXObject(version[i]);
            } catch (e) { }
        }
    }   //XMLHttpRepusest를 보낼 수 있는 객체를 xmlhttp에 저장하는 과정.
    xmlhttp.onreadystatechange = function () {  //수신 상태가 변화할때마다 호출하는 콜백함수
        if (xmlhttp.readyState == 4 && xmlhttp.status == 200) { //모든 수신이 끝났고 정상적으로 수신을 완료했을 때에 대한 if문
            var obj = xmlhttp.responseText; //php파일에서 echo로 보낸 것을 text형태로 obj에 저장.
            obj = obj.split("\test");   //\test를 기준으로 자르는 과정.(php에서 변수에 대한 값을 보내고 \test를 보냈습니다.) 여기서 입력에 \test가 오면 버그가 발생합니다.
            for (var i = 0; i < obj.length - 1;) {  //obj의 마지막은 빈 공간으로 남기 때문에 -1까지 반복하고 i에 대한 증가는 while에서 해주기 때문에 for의 마지막은 공백.
                var date_tmp = {    //하루에 대한 정보를 담는 객체. date에는 날짜 save에는 받아온 정보가 들어갑니다.
                    date: obj[i + 4],
                    save: []
                };
                while (parseInt(date_tmp.date) == parseInt(obj[i + 4]) && i < obj.length - 1) { //같은 날짜끼리 담습니다.
                    var tmp = { //date_tmp.save에 들어갈 정보 db테이블의 모든 정보가 여기에 담깁니다. id는 자동적으로 증가하는 데이터의 고유번호, date는 언제 할일인지, state는 상태정보, upper는 상위 우선순위, lower는 하위 우선순위, schedule은 할 일에 대한 정보입니다.
                        state: obj[i],
                        upper: obj[i + 1],
                        lower: obj[i + 2],
                        schedule: obj[i + 3],
                        date: obj[i + 4],
                        id: obj[i + 5]
                    };
                    date_tmp.save.push(tmp);    //date_tmp의 save속성 마지막에 tmp를 추가합니다.
                    i += 6; //i의 값을 증가시켜줍니다. 한 정보당 6개씩 들어오기 때문에 증가치가 6입니다.
                }
                var date_tmp_non = {
                    date: date_tmp.date,
                    save: []
                }   //객체는 얕은 복사를 사용하기 때문에 date_tmp의 save를 복사해줘서 정렬하지 않을 것에 대해 깊은복사를 해줍니다.
                for (var j = 0; j < date_tmp.save.length; j++) {    //date_tmp.save의 모든 값을 date_tmp_non.save로 복사합니다.
                    date_tmp_non.save.splice(0, 0, date_tmp.save[j]); //id값이 역순으로 들어오기 때문에 앞에서 부터 추가해준다.
                }
                date_tmp_non.save.sort(function (left, right) { return parseInt(left.id) - parseInt(right.id); });
                arr_non_sort.push(date_tmp_non);    //정렬하지 않은 값을 arr_non_sort에 넣습니다.
                date_tmp.save.sort(function (left, right) { //upper가 같으면 lower에 대해 오름차순으로 정렬하고 다르면 upper에 대해 오름차순으로 정렬합니다.
                    if (parseInt(left.upper) == parseInt(right.upper)) {
                        return parseInt(left.lower) - parseInt(right.lower);
                    } else {
                        return parseInt(left.upper) - parseInt(right.upper);
                    }
                });
                arr_sort.push(date_tmp);
            }
        }
    }
    xmlhttp.open("POST", "./lib/php/ajax_db.php", false);  //ajax_db에 통신을 정의합니다.
    xmlhttp.send(); //ajax_db에 통신을 합니다.

    addEvents(arr_sort);

    init(); //날짜 보여주는것 초기화.

});

function sort_btn_clicked() {   //정렬 방식이 바뀔 때 호출됩니다. 콜백함수이고 따로 들어갈 분류가 없기 때문에 
    sort_flag = !sort_flag;
    table_change(current_date.getFullYear(), current_date.getMonth() + 1, current_date.getDate());
}
