<!DOCTYPE html>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html;charset=utf-8">
<link rel="stylesheet" href="css/mdui.min.css">
<script src="js/mdui.min.js"></script>

<title>Student Informition</title>
</head>
<body>
<div class="mdui-center" style="width: 400px"><h1>Student Information</h1></div>
    
   <div class="mdui-table-fluid">
  <table class="mdui-table mdui-table-hoverable">
    <thead>
      <tr>
        <th> </th>
        <th>Name</th>
        <th>Sex</th>
        <th>Phone</th>
      </tr>
    </thead>
    <tbody>
{{#TABLE}}
      <tr>
        <td>{{field0}}</td>
        <td>{{field1}}</td>
        <td>{{field2}}</td>
        <td>{{field3}}</td>
      </tr>
{{/TABLE}}
    </tbody>
  </table>
</div>
<tr>
</tr>
<br>
<br>
<div class="mdui-center" style="width: 200px"><button class="mdui-btn mdui-btn-raised"><a href="http://192.168.43.95:8080/text.txt" download="Student Infor.txt">DownLoad</a></button></div>

</body>
</html>
