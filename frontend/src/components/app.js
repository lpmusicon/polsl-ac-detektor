import { Router } from "preact-router";

import { Routes } from "./app.routes";
import { Container } from "./container";

const App = () => (
  <Container>
    <Router>
      {Object.values(Routes).map((Route) => (
        <Route.Component path={Route.path} />
      ))}
    </Router>
  </Container>
);

export default App;
