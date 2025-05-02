async function fetchCSVData() {
  const response = await fetch('data.csv');
  const csvText = await response.text();

  const rows = csvText.split('\n').slice(1); // Skip the header row
  const data = rows.map(row => {
    const [time, Urms, Irms, P, Q, S, pf] = row.split(',');
    return {
      time,
      Urms: parseFloat(Urms),
      Irms: parseFloat(Irms),
      P: parseFloat(P),
      Q: parseFloat(Q),
      S: parseFloat(S),
      pf: parseFloat(pf),
    };
  });

  return data;
}