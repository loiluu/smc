package dk.brics.automaton;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.io.PrintWriter;

public class CombineRegexes {
	//replace some special characters to make it clean
	public static String refactor(String s){
		String ret = "";
		String symbol = "~!@#$^&*\\-+%'\";";
		for (int i = 0; i < s.length(); i++)
			if (symbol.indexOf(s.charAt(i)) >= 0)
					ret += "\\" + s.charAt(i);
			else
				ret += s.charAt(i);
		return ret;
	}
	
	
	public static void TestResult(String regex){
		RegExp reg = new RegExp(regex);
		Automaton c = reg.toAutomaton();
		BufferedReader br = null;
		
		try { 
			String sCurrentLine;
			br = new BufferedReader(new FileReader("tmp.txt"));
			while ((sCurrentLine = br.readLine()) != null) {
				if (c.run(sCurrentLine)){
					if (sCurrentLine.length() == 6)
						System.out.println(sCurrentLine);
				}
			}
		} catch (IOException e) {
			e.printStackTrace();
		} finally {
			try {
				if (br != null)br.close();
			} catch (IOException ex) {
				ex.printStackTrace();
			}
		}		
	}
	
//	public static void main(String[] args) {				
//		regex for only one charset
//		RegExp r2 = new RegExp("[a-z]*|[A-Z]*|[0-9]*|[~!@#$^&*\\-+]*");
//		regex for two charsets
//		RegExp r2 = new RegExp("([a-z]+[A-Z][a-zA-Z]*)|([A-Z]+[a-z][a-zA-Z]*)|([a-z]+[0-9][a-z0-9]*)|([0-9]+[a-z][a-z0-9]*)|([A-Z]+[0-9][A-Z0-9]*)|([0-9]+[A-Z][A-Z0-9]*)|([~!@#$^&*\\-+]+[a-z]([a-z]|[~!@#$^&*\\-+])*)|([~!@#$^&*\\-+]+[A-Z]([A-Z]|[~!@#$^&*\\-+])*)|([~!@#$^&*\\-+]+[0-9]([0-9]|[~!@#$^&*\\-+])*)|([0-9]+[~!@#$^&*\\-+]([0-9]|[~!@#$^&*\\-+])*)|([A-Z]+[~!@#$^&*\\-+]([A-Z]|[~!@#$^&*\\-+])*)|([a-z]+[~!@#$^&*\\-+]([a-z]|[~!@#$^&*\\-+])*)");
//		Regex for three charsets
//		RegExp r2 = new RegExp("[0-9]+[~!@#$^&*\\-+]([0-9]|[~!@#$^&*\\-+])*[a-z]([0-9]|[~!@#$^&*\\-+]|[a-z])*|[0-9]+[~!@#$^&*\\-+]([0-9]|[~!@#$^&*\\-+])*[A-Z]([0-9]|[~!@#$^&*\\-+]|[A-Z])*|[0-9]+[a-z]([0-9]|[a-z])*[~!@#$^&*\\-+]([0-9]|[a-z]|[~!@#$^&*\\-+])*|[0-9]+[a-z]([0-9]|[a-z])*[A-Z]([0-9]|[a-z]|[A-Z])*|[0-9]+[A-Z]([0-9]|[A-Z])*[~!@#$^&*\\-+]([0-9]|[A-Z]|[~!@#$^&*\\-+])*|[0-9]+[A-Z]([0-9]|[A-Z])*[a-z]([0-9]|[A-Z]|[a-z])*|[~!@#$^&*\\-+]+[0-9]([~!@#$^&*\\-+]|[0-9])*[a-z]([~!@#$^&*\\-+]|[0-9]|[a-z])*|[~!@#$^&*\\-+]+[0-9]([~!@#$^&*\\-+]|[0-9])*[A-Z]([~!@#$^&*\\-+]|[0-9]|[A-Z])*|[~!@#$^&*\\-+]+[a-z]([~!@#$^&*\\-+]|[a-z])*[0-9]([~!@#$^&*\\-+]|[a-z]|[0-9])*|[~!@#$^&*\\-+]+[a-z]([~!@#$^&*\\-+]|[a-z])*[A-Z]([~!@#$^&*\\-+]|[a-z]|[A-Z])*|[~!@#$^&*\\-+]+[A-Z]([~!@#$^&*\\-+]|[A-Z])*[0-9]([~!@#$^&*\\-+]|[A-Z]|[0-9])*|[~!@#$^&*\\-+]+[A-Z]([~!@#$^&*\\-+]|[A-Z])*[a-z]([~!@#$^&*\\-+]|[A-Z]|[a-z])*|[a-z]+[0-9]([a-z]|[0-9])*[~!@#$^&*\\-+]([a-z]|[0-9]|[~!@#$^&*\\-+])*|[a-z]+[0-9]([a-z]|[0-9])*[A-Z]([a-z]|[0-9]|[A-Z])*|[a-z]+[~!@#$^&*\\-+]([a-z]|[~!@#$^&*\\-+])*[0-9]([a-z]|[~!@#$^&*\\-+]|[0-9])*|[a-z]+[~!@#$^&*\\-+]([a-z]|[~!@#$^&*\\-+])*[A-Z]([a-z]|[~!@#$^&*\\-+]|[A-Z])*|[a-z]+[A-Z]([a-z]|[A-Z])*[0-9]([a-z]|[A-Z]|[0-9])*|[a-z]+[A-Z]([a-z]|[A-Z])*[~!@#$^&*\\-+]([a-z]|[A-Z]|[~!@#$^&*\\-+])*|[A-Z]+[0-9]([A-Z]|[0-9])*[~!@#$^&*\\-+]([A-Z]|[0-9]|[~!@#$^&*\\-+])*|[A-Z]+[0-9]([A-Z]|[0-9])*[a-z]([A-Z]|[0-9]|[a-z])*|[A-Z]+[~!@#$^&*\\-+]([A-Z]|[~!@#$^&*\\-+])*[0-9]([A-Z]|[~!@#$^&*\\-+]|[0-9])*|[A-Z]+[~!@#$^&*\\-+]([A-Z]|[~!@#$^&*\\-+])*[a-z]([A-Z]|[~!@#$^&*\\-+]|[a-z])*|[A-Z]+[a-z]([A-Z]|[a-z])*[0-9]([A-Z]|[a-z]|[0-9])*|[A-Z]+[a-z]([A-Z]|[a-z])*[~!@#$^&*\\-+]([A-Z]|[a-z]|[~!@#$^&*\\-+])*");
//		RegExp r2 = new RegExp("[0-9]+[~!@#$^&*\\-+]([0-9]|[~!@#$^&*\\-+])*[a-z]([0-9]|[~!@#$^&*\\-+]|[a-z])*[A-Z]([0-9]|[~!@#$^&*\\-+]|[a-z]|[A-Z])*|[0-9]+[~!@#$^&*\\-+]([0-9]|[~!@#$^&*\\-+])*[A-Z]([0-9]|[~!@#$^&*\\-+]|[A-Z])*[a-z]([0-9]|[~!@#$^&*\\-+]|[A-Z]|[a-z])*|[0-9]+[a-z]([0-9]|[a-z])*[~!@#$^&*\\-+]([0-9]|[a-z]|[~!@#$^&*\\-+])*[A-Z]([0-9]|[a-z]|[~!@#$^&*\\-+]|[A-Z])*|[0-9]+[a-z]([0-9]|[a-z])*[A-Z]([0-9]|[a-z]|[A-Z])*[~!@#$^&*\\-+]([0-9]|[a-z]|[A-Z]|[~!@#$^&*\\-+])*|[0-9]+[A-Z]([0-9]|[A-Z])*[~!@#$^&*\\-+]([0-9]|[A-Z]|[~!@#$^&*\\-+])*[a-z]([0-9]|[A-Z]|[~!@#$^&*\\-+]|[a-z])*|[0-9]+[A-Z]([0-9]|[A-Z])*[a-z]([0-9]|[A-Z]|[a-z])*[~!@#$^&*\\-+]([0-9]|[A-Z]|[a-z]|[~!@#$^&*\\-+])*|[~!@#$^&*\\-+]+[0-9]([~!@#$^&*\\-+]|[0-9])*[a-z]([~!@#$^&*\\-+]|[0-9]|[a-z])*[A-Z]([~!@#$^&*\\-+]|[0-9]|[a-z]|[A-Z])*|[~!@#$^&*\\-+]+[0-9]([~!@#$^&*\\-+]|[0-9])*[A-Z]([~!@#$^&*\\-+]|[0-9]|[A-Z])*[a-z]([~!@#$^&*\\-+]|[0-9]|[A-Z]|[a-z])*|[~!@#$^&*\\-+]+[a-z]([~!@#$^&*\\-+]|[a-z])*[0-9]([~!@#$^&*\\-+]|[a-z]|[0-9])*[A-Z]([~!@#$^&*\\-+]|[a-z]|[0-9]|[A-Z])*|[~!@#$^&*\\-+]+[a-z]([~!@#$^&*\\-+]|[a-z])*[A-Z]([~!@#$^&*\\-+]|[a-z]|[A-Z])*[0-9]([~!@#$^&*\\-+]|[a-z]|[A-Z]|[0-9])*|[~!@#$^&*\\-+]+[A-Z]([~!@#$^&*\\-+]|[A-Z])*[0-9]([~!@#$^&*\\-+]|[A-Z]|[0-9])*[a-z]([~!@#$^&*\\-+]|[A-Z]|[0-9]|[a-z])*|[~!@#$^&*\\-+]+[A-Z]([~!@#$^&*\\-+]|[A-Z])*[a-z]([~!@#$^&*\\-+]|[A-Z]|[a-z])*[0-9]([~!@#$^&*\\-+]|[A-Z]|[a-z]|[0-9])*|[a-z]+[0-9]([a-z]|[0-9])*[~!@#$^&*\\-+]([a-z]|[0-9]|[~!@#$^&*\\-+])*[A-Z]([a-z]|[0-9]|[~!@#$^&*\\-+]|[A-Z])*|[a-z]+[0-9]([a-z]|[0-9])*[A-Z]([a-z]|[0-9]|[A-Z])*[~!@#$^&*\\-+]([a-z]|[0-9]|[A-Z]|[~!@#$^&*\\-+])*|[a-z]+[~!@#$^&*\\-+]([a-z]|[~!@#$^&*\\-+])*[0-9]([a-z]|[~!@#$^&*\\-+]|[0-9])*[A-Z]([a-z]|[~!@#$^&*\\-+]|[0-9]|[A-Z])*|[a-z]+[~!@#$^&*\\-+]([a-z]|[~!@#$^&*\\-+])*[A-Z]([a-z]|[~!@#$^&*\\-+]|[A-Z])*[0-9]([a-z]|[~!@#$^&*\\-+]|[A-Z]|[0-9])*|[a-z]+[A-Z]([a-z]|[A-Z])*[0-9]([a-z]|[A-Z]|[0-9])*[~!@#$^&*\\-+]([a-z]|[A-Z]|[0-9]|[~!@#$^&*\\-+])*|[a-z]+[A-Z]([a-z]|[A-Z])*[~!@#$^&*\\-+]([a-z]|[A-Z]|[~!@#$^&*\\-+])*[0-9]([a-z]|[A-Z]|[~!@#$^&*\\-+]|[0-9])*|[A-Z]+[0-9]([A-Z]|[0-9])*[~!@#$^&*\\-+]([A-Z]|[0-9]|[~!@#$^&*\\-+])*[a-z]([A-Z]|[0-9]|[~!@#$^&*\\-+]|[a-z])*|[A-Z]+[0-9]([A-Z]|[0-9])*[a-z]([A-Z]|[0-9]|[a-z])*[~!@#$^&*\\-+]([A-Z]|[0-9]|[a-z]|[~!@#$^&*\\-+])*|[A-Z]+[~!@#$^&*\\-+]([A-Z]|[~!@#$^&*\\-+])*[0-9]([A-Z]|[~!@#$^&*\\-+]|[0-9])*[a-z]([A-Z]|[~!@#$^&*\\-+]|[0-9]|[a-z])*|[A-Z]+[~!@#$^&*\\-+]([A-Z]|[~!@#$^&*\\-+])*[a-z]([A-Z]|[~!@#$^&*\\-+]|[a-z])*[0-9]([A-Z]|[~!@#$^&*\\-+]|[a-z]|[0-9])*|[A-Z]+[a-z]([A-Z]|[a-z])*[0-9]([A-Z]|[a-z]|[0-9])*[~!@#$^&*\\-+]([A-Z]|[a-z]|[0-9]|[~!@#$^&*\\-+])*|[A-Z]+[a-z]([A-Z]|[a-z])*[~!@#$^&*\\-+]([A-Z]|[a-z]|[~!@#$^&*\\-+])*[0-9]([A-Z]|[a-z]|[~!@#$^&*\\-+]|[0-9])*");
//		RegExp r2 = new RegExp("AB(.)*");
//		Automaton c = r2.toAutomaton();
//		
//		BufferedReader br = null;
//		RegExp passReg = new RegExp("");
//		String passReg = "";
//		try { 
//			String sCurrentLine;
//			br = new BufferedReader(new FileReader("john"));
//			while ((sCurrentLine = br.readLine()) != null) {
//				if (sCurrentLine.length() == 0)
//					continue;
//				if (passReg.length() > 0)
//					passReg += "|" + refactor(sCurrentLine);
//				else
//					passReg += refactor(sCurrentLine);
//			}
//		} catch (IOException e) {
//			e.printStackTrace();
//		} finally {
//			try {
//				if (br != null)br.close();
//			} catch (IOException ex) {
//				ex.printStackTrace();
//			}
//		}		
//		System.out.println("Password: " + passReg);			
//		RegExp tmp = new RegExp("ABC(.)*");
//		
//		Automaton d = tmp.toAutomaton();
//		Automaton f = BasicOperations.intersection(c, d);
//		f = Automaton.minimize(f);
//		f.reduce();
//		System.out.println("New Automata: " + f);
//		String reg = BasicOperations.toRegex(f);	
//		System.out.println("New regex: " + reg);
//		
//	}
	public static void main(String[] args) {
		BufferedReader br = null;
		Automaton finalAutomaton = null;		
		try { 
			String sCurrentLine;
			br = new BufferedReader(new FileReader("regex_query.input"));
			while ((sCurrentLine = br.readLine()) != null) {
				if (sCurrentLine.length() == 0)
					continue;
				
				RegExp newReg = new RegExp(sCurrentLine);				
				if (finalAutomaton == null)
					finalAutomaton = newReg.toAutomaton();
				else{
					Automaton newAutomaton = newReg.toAutomaton();
					finalAutomaton = BasicOperations.intersection(finalAutomaton, newAutomaton);					
				}				
				finalAutomaton = Automaton.minimize(finalAutomaton);
			}
		} catch (IOException e){
			e.printStackTrace();
		} finally {
			try {
				if (br != null)br.close();
			} catch (IOException ex) {
				ex.printStackTrace();
			}
		}		
		String reg = BasicOperations.toRegex(finalAutomaton);
		
		PrintWriter writer;
		try {
			writer = new PrintWriter("regex_query.output", "UTF-8");
			writer.println(reg);		
			writer.close();
		} catch (Exception e) {	
			e.printStackTrace();
		}
	}
}
