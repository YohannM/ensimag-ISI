package arithmetique;

import java.util.HashMap;
import java.util.Map;

public class Env {
    Map<String, Double> map = new HashMap<String, Double>();

    public void associer(String nom, double valeur) {
        this.map.put(nom, valeur);
    }

    public double obtenirValeur(String nom) {
        return this.map.get(nom);
    }

    @Override
    public String toString() {
        return map.toString();
    }
}
