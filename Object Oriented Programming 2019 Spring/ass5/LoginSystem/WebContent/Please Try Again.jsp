<!DOCTYPE html>
<html>

	<head>
		<meta charset="ISO-8859-1">
    	<title> Information Incorrect </title>
	</head>

	<body>
		<h1> Please Try Again </h1>
		<p> Either your user name or password is incorrect. Please try again. </p>
		
		<form action = "LoginServlet" method = "post">
			User Name : <input type = "text" name = "username" value = ""> <br>
			Password : <input type = "text" name = "password" value = "">
			<input type = "submit" value = "Login">
		</form>
		
		<a href = "Create New Account.jsp" > Create Account </a>
		
	</body>

</html>
