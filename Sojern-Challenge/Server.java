package rest.chandu.jersey.web;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Iterator;
import java.util.Map;
import java.util.StringTokenizer;

import javax.ws.rs.Consumes;
import javax.ws.rs.GET;
import javax.ws.rs.POST;
import javax.ws.rs.Path;
import javax.ws.rs.PathParam;
import javax.ws.rs.Produces;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.Response;

import org.codehaus.jettison.json.JSONException;
import org.codehaus.jettison.json.JSONObject;


// The class registers its methods for the HTTP GET request using the @GET annotation. 
// Using the @Produces annotation, it defines that it can deliver several MIME types,
// text, XML and HTML. 

// Sets the path to base URL + /hello

@Path("/hello")
public class Server{
	
  private SingletonStore ss;

  static Map<String, String> map = null;
  // Path to backup file.
  static String file = "BackUP.txt";

  public Server()
  {
	 try
	 {
		 ss = new SingletonStore();
		 map = ss.getInstance();
		 System.out.print(map.get("count"));
	 }catch(Exception e)
	 {
	     System.out.print("Exception");
	 }
  }
  
  // This method is called if TEXT_PLAIN is request
  
  @GET
  @Path("/get/{i}")
  @Produces(MediaType.TEXT_PLAIN)
  public Response sayPlainTextHello(@PathParam("i") String i) {
	  if (map.get(i) != null)
		  return Response.ok(map.get(i)).build();
	  else
		  return Response.status(404).build();
  }
  
//This method is called if JSON is request
 @GET
 @Path("/multiget/{j}")
 @Produces(MediaType.APPLICATION_JSON)
 public Response sayJsonHello(@PathParam("j") String j) {
	  StringTokenizer st = new StringTokenizer(j, ",");
      String res = "{" + "\n";
	  while(st.hasMoreElements())
	  {
		  String next = st.nextToken();
		  if(map.get(next) == null)
		      return Response.status(404).build();
		  res += next + " : " + map.get(next)+ "\n";
	  }
	  res += "}";
	  return Response.ok(res).build();
 }
 
//This method is called if JSON is request
@POST
@Path("/set")
@Consumes(MediaType.APPLICATION_JSON)
public Response createPlainHello(JSONObject pairs) {
	try {
		storeJsonData(pairs);
	} catch (JSONException e) {
		e.printStackTrace();
	}
  return Response.status(201).entity(pairs).build();
}

/*
 * Insert json data into map and backup 
 */

public static void storeJsonData(JSONObject json) throws JSONException{
    Iterator<String> keys = json.keys();
    while(keys.hasNext()){
        String key = keys.next();
        String val = json.getString(key);
        
        if(val != null){
            map.put(key,val);
            try {
    			BufferedWriter out = new BufferedWriter(new FileWriter(file,true));
    			out.write(key + " : " + val);
    			out.newLine();
    			out.close();
    		} catch (IOException e) {
    			e.printStackTrace();
    		}
        }
    }
}
} 

