<!DOCTYPE html>
<html>

	<head>
		<meta charset="ISO-8859-1">
    	<title> Create Account </title>
	</head>

	<body>
		<h1> The Name <%= request.getParameter("username") %> Is Already In Use </h1>
		<p> Please enter another name and password. </p>
		
		<form action = "CreateServlet" method = "post">
			User Name : <input type = "text" name = "username" value = ""> <br>
			Password : <input type = "text" name = "password" value = "">
			<input type = "submit" value = "Login">
		</form>
		
	</body>

</html>
