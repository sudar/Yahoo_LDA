import java.io.BufferedReader;
import java.io.FileReader;
import java.io.InputStreamReader;
import java.io.IOException;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class Tokenizer {
    public static final Pattern LEX_ALPHA = Pattern.compile ("\\p{Alpha}+");

    public static String clean(String text_line){
        text_line = text_line.replaceAll("\\p{Punct}"," ");
        text_line = text_line.replaceAll("\\p{Space}+", " ");
        return text_line.toLowerCase().trim();
    }
   
    /**
     * @param args
     * @throws IOException 
     */
    public static void main(String[] args) throws IOException {
        BufferedReader br = null;
        if(args.length==0)
          br = new BufferedReader(new InputStreamReader(System.in));
        else
          br = new BufferedReader(new FileReader(args[0]));
        String s = null;
        int doc_ind = 0;
        while((s=br.readLine())!=null){
	    //System.err.print(++doc_ind + " ");
            int firstSpace = s.indexOf(' ');
	    if(firstSpace<0 || firstSpace>=s.length()-1) continue;
            String url = s.substring(0,firstSpace);
            int secSpace = s.indexOf(' ',firstSpace+1);
	    if(secSpace<1 || secSpace>=s.length()-1) continue;
            String label = s.substring(firstSpace+1,secSpace);
            String cont = s.substring(secSpace+1,s.length());
            StringBuffer sb = new StringBuffer(url + " " + label + " ");

	    cont = clean(cont);            
            Matcher matcher = LEX_ALPHA.matcher(cont.subSequence(0, cont.length()));
            String matchText = null;
            while(matcher.find()){
                matchText = matcher.group();
                if(matchText.length()>=100) continue;
                if(!matchText.equals("")){
                    sb.append(matchText);
                    sb.append(" ");
                }
            }
            sb.deleteCharAt(sb.length()-1);
	    System.out.println(sb.toString());
        }
    }

}
