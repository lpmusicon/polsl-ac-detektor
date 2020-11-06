import styled from "styled-components";

export const SSID = styled.div`
  padding: 12px;
  border-radius: 7px;
  border: 1px solid ${(p) => (p.active ? "#44d194" : "transparent")};
  margin: -1px;
  cursor: pointer;
  position: relative;
  transition: border 0.3s ease-in-out;
  display: flex;
  justify-content: space-between;
  align-items: center;

  &::before {
    content: "";
    position: absolute;
    border-radius: 7px;
    display: block;
    top: 0;
    left: 0;
    right: 0;
    bottom: 0;
    background: linear-gradient(
      97.49deg,
      #083b4b 6.05%,
      rgba(7, 32, 44, 0) 94.12%
    );
    mix-blend-mode: lighten;
    opacity: ${(p) => (p.active ? 1 : 0)};
    transition: opacity 0.3s ease-in-out;
  }

  &:hover {
    border: 1px solid #44d194;
  }

  &:hover::before {
    opacity: 1;
  }
`;
