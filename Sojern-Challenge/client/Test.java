package rest.chandu.jersey.web.client;

import java.net.URI;
import java.util.Random;
import java.util.Scanner;
import java.util.StringTokenizer;

import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.UriBuilder;

import org.codehaus.jettison.json.JSONObject;
import com.sun.jersey.api.client.Client;
import com.sun.jersey.api.client.ClientResponse;
import com.sun.jersey.api.client.WebResource;
import com.sun.jersey.api.client.config.ClientConfig;
import com.sun.jersey.api.client.config.DefaultClientConfig;


public class Test {
	 
  	public static void main(String[] args) {
		try
		{
	    ClientConfig config = new DefaultClientConfig();
	    Client client = Client.create(config);
	    WebResource service = client.resource(getBaseURI());
	    Scanner sc = new Scanner(System.in);
	    while(true)
	    {
	    	String comm = sc.next();
	    	StringTokenizer st = new StringTokenizer(comm,"/");

	    	String mult_keys = "";
	    	
	    	// "Key Values format taken as k1-v1, k2-v2 etc"
	    	
	    	switch(st.nextToken())
	    	{
	    	case "get" :
	    		// Get value for this key
	    		String key = st.nextToken();
	    	    ClientResponse res = service.path("rest/hello").path("get").path(key).accept(MediaType.TEXT_PLAIN).get(ClientResponse.class);
	    	    // Response messages upon success/failure look up
	    	    if(res.getStatus() == 200)
	    	    {
	    	    	System.out.println(res.getEntity(String.class));
	    	    }
	    	    else
	    	    	System.out.println("Response Status:"+res.getStatus());
	    	    break;
	    	case "multiget" :
	    		while(st.hasMoreTokens())
	    		{
	    			String next = st.nextToken();
	    		    mult_keys += next + ",";
	    		}
	    		// Get values for the keys
	    		ClientResponse res1 = service.path("rest/hello").path("multiget").path(mult_keys).accept(MediaType.APPLICATION_JSON).get(ClientResponse.class);
	    		// Response messages upon success/failure look up
	    		if(res1.getStatus() == 200)
	    	    {
	    	    	System.out.println(res1.getEntity(String.class));
	    	    }
	    	    else
	    	    	System.out.println("Response Status:"+res1.getStatus());
	    	    break;
	    	case "set" :
	    		// [1-5] Random Key-Value Pairs 
	    		Random r = new Random(); 
	    		int no_ins = r.nextInt(5) + 1;
	    		JSONObject input = new JSONObject();
	    		// [0-100] random key-value indexes
	    		for(int k =0; k<no_ins; k++)
	    		{
	    			int rand = r.nextInt(100);  
	    		    String temp_key = "k" +rand;
	    		    String temp_val = "v" + rand;
	    		    input.put(temp_key, temp_val);
	    		}
	    	    ClientResponse response = service.path("rest/hello").path("set").type("application/json").post(ClientResponse.class, input);
	    		if (response.getStatus() != 201) {
	    			throw new RuntimeException("Failed : HTTP error code : "
	    			     + response.getStatus());
	    		}
	    		JSONObject output = response.getEntity(JSONObject.class);
	    		System.out.println(output);
	    		break;
	    	case "exit" :
	    		sc.close();
	    		System.exit(0);
	    	default:
	    		System.out.println("Enter valid Path");
	    	}
	    }
		}catch(Exception e)
		{
			e.printStackTrace();
		}
    

  	}
  

  	private static URI getBaseURI() {
    	return UriBuilder.fromUri("http://localhost:8080/rest.chandu.jersey.web").build();
  	}

} 
