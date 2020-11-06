import styled from "styled-components";

export const FieldWrapper = styled.div`
  border: 1px solid #becbd8;
  border-radius: 0;
  border-bottom: 0;
  ${(p) => (p.first ? "border-radius: 5px 5px 0 0;" : null)}
  ${(p) => (p.last ? "border-radius: 0 0 5px 5px;" : null)}
  ${(p) => (p.last ? "border-bottom: 1px solid #becbd8;" : null)}
  ${(p) => (p.only ? "border-radius: 5px !important;" : null)}
`;
