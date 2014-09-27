/*
 *	EverNote Challenge
 * 	@Author: Sarath
 */

import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.util.*;
import java.io.*;
import java.text.*;

class Note implements Comparable < Note > {
	String GUID;
	java.util.List < String > TAGS;
	String Created;
	String Content;
	public Note() {
		TAGS = new java.util.ArrayList < String > ();
	}

	public int compareTo(Note note) {
		try {
			if (checkDate(this.Created, note.Created) == 1) return 1;
			else if (checkDate(this.Created, note.Created) == 0) return 0;
		} catch (Exception e) {
			e.printStackTrace();
		}
		return -1;
	}

	public int checkDate(String s1, String s2) throws ParseException {
		String dateString1 = s1.substring(0, 10);
		String dateString2 = s2.substring(0, 10);
		SimpleDateFormat format = new SimpleDateFormat("yyyy-MM-dd");
		Date date1 = format.parse(dateString1);
		Date date2 = format.parse(dateString2);

		if (date1.compareTo(date2) < 0) {
			return -1;
		} else if (date1.compareTo(date2) == 0) {
			format = new SimpleDateFormat("HH:mm:ss");
			String timeString1 = s1.substring(11, s1.length() - 1);
			String timeString2 = s2.substring(11, s2.length() - 1);
			Date time1 = format.parse(timeString1);
			Date time2 = format.parse(timeString2);
			if (time1.compareTo(time2) < 0) return -1;
			if (time1.compareTo(time2) == 0) return 0;
			return 1;
		}
		return 1;
	}
}

public class Solution {
	public static void main(String[] args) {
		//Read Input
		try {
			BufferedReader br = new BufferedReader(new InputStreamReader(System. in ));
			java.util.Map < String, Note > map = new java.util.HashMap < String, Note > ();
			String line = br.readLine();
			while (line != null) {
				Note note = null;
				switch (line) {
					case "CREATE":
						note = readXMLFile(br);
						map.put(note.GUID, note);
						break;
					case "UPDATE":
						note = readXMLFile(br);
						deleteNoteEntry(note.GUID, map);
						map.put(note.GUID, note);
						break;
					case "DELETE":
						String note_id = br.readLine();
						deleteNoteEntry(note_id, map);
						break;
					case "SEARCH":
						String element = br.readLine();
						ArrayList < Note > hits = searchList(element, map);
						if (!hits.isEmpty()) {
							Collections.sort(hits);
							String output = "";
							for (Note t: hits)
							output += t.GUID + ",";
							output = output.substring(0, output.length() - 1);
							System.out.println(output);
						} else System.out.println();
						break;
				}
				line = br.readLine();
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	/*
	 *	Method to read the XML file from STDIN
	 */
	public static Note readXMLFile(BufferedReader br) {
		String xml_file = "";
		Note note = null;
		try {
			String input = br.readLine();
			note = new Note();
			String xml_string = "";
			while (!input.equals("</note>")) {
				xml_string += input + '\n';
				input = br.readLine();
			}
			xml_string += input;
			parseNote(xml_string, note);
		} catch (Exception e) {
			e.printStackTrace();
		}
		return note;
	}

	/*
	 *	Method to fill Node class from XML
	 */

	public static void parseNote(String input, Note note) {
		Pattern regex = Pattern.compile("<note>(.*?)</note>", Pattern.DOTALL);
		Matcher matcher = regex.matcher(input);
		Pattern regex2 = Pattern.compile("<([^<>]+)>([^<>]+)</\\1>");
		if (matcher.find()) {
			String Elements = matcher.group(1);
			Matcher matcher2 = regex2.matcher(Elements);
			while (matcher2.find()) {
				if (matcher2.group(1).equals("guid")) note.GUID = matcher2.group(2);
				else if (matcher2.group(1).equals("tag")) note.TAGS.add(matcher2.group(2));
				else if (matcher2.group(1).equals("created")) note.Created = matcher2.group(2);
				else if (matcher2.group(1).equals("content")) note.Content = matcher2.group(2);
			}
		}
	}

	/*	
	 *	method to delete note entry
	 */

	public static void deleteNoteEntry(String note_id, java.util.Map < String, Note > map) {
		map.remove(note_id);
	}

	/*
	 *	Method to search keyword from the Notes
	 */
	public static ArrayList < Note > searchList(String element, java.util.Map < String, Note > map) {
		ArrayList < Note > list = new ArrayList < Note > ();
		String[] inp = element.split("\\s+");
		for (String s: map.keySet()) {
			Note this_note = map.get(s);
			boolean flag = true;
			for (String p: inp) {
				if (p.startsWith("tag:")){
					p = p.substring(4, p.length());
					if (p.charAt(p.length() - 1) == '*') {
						p = p.substring(0, p.length() - 1);
						int check = 0;
						for (String tag: this_note.TAGS) {
							if (hasPrefix(tag, p)) check = 1;
						}
						if (check == 0) flag = false;
					} else {
						int check = 0;
						for (String r: this_note.TAGS) {
							if (hasWord(r, p)) check = 1;
						}
						if (check == 0) flag = false;
					}
				} else if(p.startsWith("created:")) {
					p = p.substring(8, p.length());
					try {
						if (isEarlierDate(this_note.Created, p)) flag = false;
					} catch (ParseException e) {
						e.printStackTrace();
					}
				} else if (p.charAt(p.length() - 1) == '*') {
					p = p.substring(0, p.length() - 1);
					if (!hasPrefix(this_note.Content, p)) flag = false;
				} else {
					if (!hasWord(this_note.Content, p)) flag = false;
				}
			}
			if (flag) list.add(this_note);
		}
		return list;
	}

    /*
    *   Method to look up prefix
    */
	public static boolean hasPrefix(String s1, String s2) {
		s1 = s1.toLowerCase();
		s2 = s2.toLowerCase();
		String regex = "\\b"+s2+"\\w*\\b";
		Pattern p = Pattern.compile(regex);
		Matcher m = p.matcher(s1);
		if (m.find()) {
			return true;
		}
		return false;
	}

    /*
    *   Method to look up whole word
    */
	public static boolean hasWord(String s1, String s2) {
		s1 = s1.toLowerCase();
		s2 = s2.toLowerCase();
		String regex = ".*\\b" + s2 + "\\b.*";
		Pattern p = Pattern.compile(regex);
		Matcher m = p.matcher(s1);
		if (m.find()) {
			//System.out.println(m.group());
			return true;
		}
		return false;
	}

    /*
    *   Method to compare ISO dates
    */
	public static boolean isEarlierDate(String s1, String s2) throws ParseException {
		String dateString1 = s1.substring(0, 10);
		String dateString2 = s2.substring(0, 4) + "-" + s2.substring(4, 6) + "-" + s2.substring(6, s2.length());
		SimpleDateFormat format = new SimpleDateFormat("yyyy-MM-dd");
		Date date1 = format.parse(dateString1);
		Date date2 = format.parse(dateString2);

		if (date1.compareTo(date2) <= 0) {
			return true;
		}
		return false;
	}
}
