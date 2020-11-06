import styled from "styled-components";

export const CardContainer = styled.div`
  display: grid;
  grid-auto-flow: row;
  grid-auto-columns: 100%;
  gap: 16px;
  overflow-y: auto;
  max-height: calc(100% - 104px);
  margin-bottom: 16px;
  position: relative;
  &::before {
    position: sticky;
    top: 0;
    left: 0;
    right: 0;
    height: 24px;
    margin: -24px 0;
    background: linear-gradient(#07202c, transparent);
    content: "";
    display: block;
  }
  &::after {
    position: sticky;
    bottom: 0;
    left: 0;
    right: 0;
    height: 24px;
    margin: -24px 0 0;
    background: linear-gradient(transparent, #07202c);
    content: "";
    display: block;
  }
`;
