<!DOCTYPE html>
<%@page import = "Logic.Constants"%>
<%@page import = "Logic.Product"%>
<%@ page import = "java.util.Iterator"%>
<%@ page import = "Logic.ProductManager"%>
<%@ page import = "java.util.Map"%>

<html>

	<head>
		<meta charset="ISO-8859-1">
    	<title> Student Store </title>
	</head>

	<body>
		<h1> Student Store </h1>
		<p> Items available : </p>
			
			<ul>	
				<%
					ProductManager p = (ProductManager) request.getServletContext().getAttribute(Constants.MANAGER_ATTR);
					Iterator<Map.Entry<String, Product>> it = p.getProducts();
					
					while(it.hasNext()) {
						Product cur = it.next().getValue();
						String url = " \"show-product.jsp?id=" + cur.getId() + "\" ";
						out.println("<li> <a href = " + url + "> " + cur.getName() + " </a> </li>");
					}
				%>
			</ul>
		
	</body>

</html>
